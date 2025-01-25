#include "splitter.h"

#include <ctype.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "mbtstr/str.h"
#include "shard.h"
#include "utils/logger.h"
#include "utils/xalloc.h"

DEFINE_VARIABLES;
DEFINE_REDIRS;
DEFINE_OPERATORS;

#define SCE SHARD_CONTEXT_EXPANSION
#define SCEPP SHARD_CONTEXT_EXPANSION_POPPED_PARENTHESIS
#define PUSH_AND_READ(str) mbt_str_pushc(str, stream_read(ctx->stream));

/*
 * ---------------------------------------------------------------------------
 * PROTOTYPES
 * ---------------------------------------------------------------------------
 */
static struct shard *splitter_handle_double_quotes(struct splitter_ctx *ctx,
                                                   struct mbt_str *str);

static struct shard *splitter_handle_expansion(struct splitter_ctx *ctx,
                                               struct mbt_str *str,
                                               bool popped);

static void splitter_handle_backslash(struct splitter_ctx *ctx,
                                      struct mbt_str *str);

static bool splitter_read_between(struct splitter_ctx *ctx, struct mbt_str *str,
                                  char left, char right);

static bool splitter_read_until(struct splitter_ctx *ctx, struct mbt_str *str,
                                char c);

static bool is_valid_identifier(struct mbt_str *str);

static bool list_any_begins_with(const char *list[], char c);

static void discard_comment(struct splitter_ctx *ctx);

static struct shard *splitter_prepare_quotes(struct splitter_ctx *ctx,
                                             struct mbt_str *str, char c)
{
    if (NOT_EMPTY(str))
    {
        return shard_init(str, true, SHARD_WORD, SHARD_UNQUOTED);
    }

    if (c == '\\')
    {
        splitter_handle_backslash(ctx, str);
        return shard_init(str, true, SHARD_WORD, SHARD_BACKSLASH_QUOTED);
    }

    if (c == '\'')
    {
        stream_read(ctx->stream);
        splitter_read_until(ctx, str, '\'');
        char end_quote = stream_read(ctx->stream);

        if (end_quote != '\'')
        {
            warnx("Syntax error: unmatched simple quote");
            ctx->err = true;
            mbt_str_free(str);
            return NULL;
        }

        return shard_init(str, true, SHARD_WORD, SHARD_SINGLE_QUOTED);
    }

    if (c == '"')
    {
        stream_read(ctx->stream); // Pop the quote
        splitter_ctx_expect(ctx, SHARD_CONTEXT_DOUBLE_QUOTES);
        return splitter_handle_double_quotes(ctx, str);
    }

    return NULL;
}

static struct shard *splitter_shard_operator(struct mbt_str *str)
{
    if (shard_is_any_operator(str))
    {
        return shard_init(str, false, SHARD_OPERATOR, SHARD_UNQUOTED);
    }
    if (shard_is_redir(str))
    {
        return shard_init(str, false, SHARD_REDIR, SHARD_UNQUOTED);
    }

    return shard_init(str, true, SHARD_WORD, SHARD_UNQUOTED);
}

static struct shard *splitter_resume(struct splitter_ctx *ctx,
                                     struct mbt_str *str)
{
    struct splitter_ctx_exp *exp = stack_peek(ctx->expect);

    if (exp->value == SCE || exp->value == SCEPP)
    {
        enum shard_ctx_type exp_meta = exp->value;
        free(stack_pop(ctx->expect));

        return splitter_handle_expansion(ctx, str, exp_meta == SCEPP);
    }
    else if (exp->value == SHARD_CONTEXT_DOUBLE_QUOTES)
    {
        return splitter_handle_double_quotes(ctx, str);
    }

    return NULL;
}

static struct shard *splitter_handle_newlines(struct splitter_ctx *ctx,
                                              struct mbt_str *str, char c)
{
    if (NOT_EMPTY(str))
    {
        return shard_init(str, true, SHARD_WORD, SHARD_UNQUOTED);
    }
    else
    {
        mbt_str_pushc(str, c); // @ref1
        stream_read(ctx->stream);
    }

    return shard_init(str, false, SHARD_WORD, SHARD_UNQUOTED);
}

static struct shard *splitter_prepare_expansion(struct splitter_ctx *ctx,
                                                struct mbt_str *str, char c)
{
    if (NOT_EMPTY(str))
    {
        return shard_init(str, true, SHARD_WORD, SHARD_UNQUOTED);
    }

    if (c == '$')
    {
        stream_read(ctx->stream);
    }

    return splitter_handle_expansion(ctx, str, false);
}

static struct shard *splitter_handle_spaces(struct splitter_ctx *ctx,
                                            struct mbt_str *str, char c)
{
    if (NOT_EMPTY(str))
    {
        return shard_init(str, true, SHARD_WORD, SHARD_UNQUOTED);
    }

    while ((c = stream_peek(ctx->stream)) > 0 && isspace(c))
    {
        stream_read(ctx->stream);
    }

    return DELIMITER(str);
}

/*
 * ---------------------------------------------------------------------------
 * PROTOTYPES
 * ---------------------------------------------------------------------------
 */

static struct shard *splitter_next(struct splitter_ctx *ctx,
                                   struct mbt_str *str)
{
    char c;
    while (!ctx->err && (c = stream_peek(ctx->stream)) > 0)
    {
        // Case 1: EOF handled by exiting the loop
        if (shard_is_any_operator(str) || shard_is_redir(str))
        {
            mbt_str_pushc(str, c);
            if (shard_is_any_operator(str) || shard_is_redir(str)) // Case 2
            {
                stream_read(ctx->stream);
            }
            else // Case 3
            {
                mbt_str_pop(str); // Not an operator -> We delimit

                return splitter_shard_operator(str);
            }
        }

        else if (strchr("\\\"\'", c))
        {
            struct shard *shard = splitter_prepare_quotes(ctx, str, c);
            if (shard)
            {
                return shard;
            }

            break;
        }

        else if (c == '`' || c == '$')
        {
            return splitter_prepare_expansion(ctx, str, c);
        }

        // Case 5: Expansions
        // Case 6: New operator
        else if (list_any_begins_with(OPERATORS, c)) // Case 6: matched
        {
            if (NOT_EMPTY(str))
            {
                return shard_init(str, true, SHARD_WORD, SHARD_UNQUOTED);
            }

            PUSH_AND_READ(str);
        }

        else if (list_any_begins_with(REDIRS, c))
        {
            // We allow at most 1 digit before redirection
            if (str->size > 1 || (str->size && !isdigit(str->data[0])))
            {
                break;
            }

            PUSH_AND_READ(str);
        }

        // Case 7: Newlines
        else if (c == '\n')
        {
            return splitter_handle_newlines(ctx, str, c);
        }

        // Case 8: delimiter
        else if (isspace(c))
        {
            return splitter_handle_spaces(ctx, str, c);
        }

        // Case 9: existing word
        else if (NOT_EMPTY(str))
        {
            PUSH_AND_READ(str);
        }

        // Case 10: comments
        else if (c == '#')
        {
            discard_comment(ctx);
        }

        // Case 11: new word: keep looping
        else
        {
            PUSH_AND_READ(str);
        }
    }

    if (!ctx->err && NOT_EMPTY(str))
    {
        return splitter_shard_operator(str);
    }

    if (!ctx->err)
    {
        mbt_str_free(str);
    }
    return NULL;
}

struct shard *splitter_peek(struct splitter_ctx *ctx)
{
    if (!ctx->cache)
    {
        struct mbt_str *str = mbt_str_init(64);
        ctx->cache = ctx->expect->size ? splitter_resume(ctx, str)
                                       : splitter_next(ctx, str);
    }

    return ctx->cache;
}

struct shard *splitter_pop(struct splitter_ctx *ctx)
{
    struct shard *shard = splitter_peek(ctx);
    ctx->cache = NULL;
    return shard;
}

static struct shard *splitter_handle_sh_expansion(struct splitter_ctx *ctx,
                                                  struct mbt_str *str,
                                                  enum shard_quote_type type,
                                                  bool popped)
{
    char c = stream_peek(ctx->stream);
    bool is_arith = c == '(' && popped;
    if (!popped || is_arith)
    {
        stream_read(ctx->stream);
    }

    while ((c = stream_peek(ctx->stream)) > 0 && c == ' ')
    {
        stream_read(ctx->stream);
    }

    if (splitter_read_between(ctx, str, '(', ')'))
    {
        splitter_ctx_error(ctx, "expected )");
        return NULL;
    }

    if (is_arith || !popped)
    {
        c = stream_read(ctx->stream);

        if (c != ')')
        {
            splitter_ctx_error(ctx, "expected )");
            return NULL;
        }
    }

    return shard_init(
        str, true, is_arith ? SHARD_EXPANSION_ARITH : SHARD_EXPANSION_SUBSHELL,
        type);
}

static struct shard *splitter_handle_var_expansion(struct splitter_ctx *ctx,
                                                   struct mbt_str *str,
                                                   enum shard_quote_type type)
{
    char c;
    while ((c = stream_peek(ctx->stream)) > 0)
    {
        mbt_str_pushc(str, c);
        for (size_t i = 0; VARIABLES[i]; i++)
        {
            if (strcmp(str->data, VARIABLES[i]) == 0)
            {
                stream_read(ctx->stream);
                return shard_init(str, true, SHARD_EXPANSION_VARIABLE, type);
            }
        }

        if (!is_valid_identifier(str))
        {
            mbt_str_pop(str);
            if (!str->size)
            {
                break;
            }

            return shard_init(str, true, SHARD_EXPANSION_VARIABLE, type);
        }

        stream_read(ctx->stream);
    }

    if (!str->size)
    {
        mbt_str_pushc(str, '$');
        return shard_init(str, true, SHARD_WORD, type);
    }

    return shard_init(str, true, SHARD_EXPANSION_VARIABLE, type);
}

// @TIDY
static struct shard *splitter_handle_expansion(struct splitter_ctx *ctx,
                                               struct mbt_str *str, bool popped)
{
    enum shard_quote_type type = splitter_ctx_get_active_quote(ctx);

    char c = stream_peek(ctx->stream);
    if (c == '{')
    {
        stream_read(ctx->stream);
        if (splitter_read_between(ctx, str, '{', '}'))
        {
            splitter_ctx_error(ctx, "expected }");
            goto error;
        }

        stream_read(ctx->stream);
        return shard_init(str, true, SHARD_EXPANSION_VARIABLE, type);
    }
    else if (c == '(' || popped)
    {
        struct shard *shard = splitter_handle_sh_expansion(ctx, str, c, popped);
        if (!shard)
        {
            goto error;
        }
        return shard;
    }
    else if (c == '`')
    {
        stream_read(ctx->stream);
        if (splitter_read_until(ctx, str, '`'))
        {
            splitter_ctx_error(ctx, "expected `");
            goto error;
        }

        stream_read(ctx->stream);
        return shard_init(str, true, SHARD_EXPANSION_SUBSHELL, type);
    }
    else
    {
        return splitter_handle_var_expansion(ctx, str, type);
    }

error:
    mbt_str_free(str);
    return NULL;
}

#define SGS SHARD_GLOBBING_STAR
#define GLOB_TYPE(c) c == '*' ? SGS : SHARD_GLOBBING_QUESTIONMARK

static struct shard *splitter_handle_double_quotes(struct splitter_ctx *ctx,
                                                   struct mbt_str *str)
{
    struct splitter_ctx_exp *expected = stack_peek(ctx->expect);

    char c;
    while ((c = stream_peek(ctx->stream)) > 0)
    {
        switch (c)
        {
        case '"':
            if (expected->value != SHARD_CONTEXT_DOUBLE_QUOTES)
            {
                goto error;
            }

            stream_read(ctx->stream);
            free(stack_pop(ctx->expect));
            return shard_init(str, true, SHARD_WORD, SHARD_DOUBLE_QUOTED);

        case '$':
            if (NOT_EMPTY(str))
            {
                return shard_init(str, true, SHARD_WORD, SHARD_DOUBLE_QUOTED);
            }

            stream_read(ctx->stream); // Discard
            return splitter_handle_expansion(ctx, str, false);

        case '`':
            return splitter_handle_expansion(ctx, str, false);

        case '?':
        case '*':
            if (NOT_EMPTY(str))
            {
                return shard_init(str, true, SHARD_WORD, SHARD_DOUBLE_QUOTED);
            }

            mbt_str_pushc(str, stream_read(ctx->stream));
            return shard_init(str, true, GLOB_TYPE(c), SHARD_DOUBLE_QUOTED);

        case '\\':
            if (NOT_EMPTY(str))
            {
                return shard_init(str, true, SHARD_WORD, SHARD_DOUBLE_QUOTED);
            }
            splitter_handle_backslash(ctx, str);
            return shard_init(str, true, SHARD_WORD, SHARD_BACKSLASH_QUOTED);

        case 0:
        case EOF:
            mbt_str_free(str);
            return NULL;

        default:
            mbt_str_pushc(str, stream_read(ctx->stream));
            break;
        }
    }

error:
    splitter_ctx_error(ctx, "expected double quote");
    mbt_str_free(str);
    return NULL;
}

static void splitter_handle_backslash(struct splitter_ctx *ctx,
                                      struct mbt_str *str)
{
    stream_read(ctx->stream);

    char c = stream_read(ctx->stream);
    if (c > 0 && c != '\n')
    {
        mbt_str_pushc(str, c);
    }
}

/*
 * ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------
 * UTILS
 * ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------
 */
static bool splitter_read_until(struct splitter_ctx *ctx, struct mbt_str *str,
                                char c)
{
    char ch;
    while ((ch = stream_peek(ctx->stream)) > 0)
    {
        if (ch == c)
        {
            break;
        }

        mbt_str_pushc(str, ch);
        stream_read(ctx->stream);
    }

    return ch <= 0;
}

static bool splitter_read_between(struct splitter_ctx *ctx, struct mbt_str *str,
                                  char left, char right)
{
    char c;
    int pcount = 0;
    while ((c = stream_peek(ctx->stream)) > 0)
    {
        if (c == '\\')
        {
            splitter_handle_backslash(ctx, str);
        }

        if (c == left)
        {
            pcount++;
        }

        if (c == right)
        {
            if (!pcount)
            {
                break;
            }

            pcount--;
        }

        mbt_str_pushc(str, c);
        stream_read(ctx->stream);
    }

    return c <= 0;
}

static bool list_any_begins_with(const char *list[], char c)
{
    for (size_t i = 0; list[i]; i++)
    {
        if (list[i][0] == c)
        {
            return true;
        }
    }

    return false;
}

static bool is_valid_identifier(struct mbt_str *str)
{
    if (!str->size)
    {
        return false;
    }

    if (isdigit(*str->data))
    {
        for (size_t i = 0; i < str->size; i++)
        {
            if (!isdigit(str->data[i]))
            {
                return false;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < str->size; i++)
        {
            char c = str->data[i];

            if (!isalnum(c) && c != '_')
            {
                return false;
            }
        }
    }

    return true;
}

static void discard_comment(struct splitter_ctx *ctx)
{
    char c;
    while ((c = stream_peek(ctx->stream)) > 0 && c != '\n')
    {
        stream_read(ctx->stream);
    }
}
