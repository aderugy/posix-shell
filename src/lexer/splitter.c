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

#define BREAK 1
#define CONTINUE 2

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
/*
 * ---------------------------------------------------------------------------
 * PROTOTYPES
 * ---------------------------------------------------------------------------
 */
// @TIDY

static struct shard *ctx_expect(struct splitter_ctx *ctx, int *end,
                                struct mbt_str **str)
{
    struct splitter_ctx_exp *exp = stack_peek(ctx->expect);

    if (exp->value == SHARD_CONTEXT_EXPANSION
        || exp->value == SHARD_CONTEXT_EXPANSION_POPPED_PARENTHESIS)
    {
        enum shard_ctx_type exp_meta = exp->value;
        free(stack_pop(ctx->expect));

        return splitter_handle_expansion(
            ctx, *str, exp_meta == SHARD_CONTEXT_EXPANSION_POPPED_PARENTHESIS);
    }
    else if (exp->value == SHARD_CONTEXT_DOUBLE_QUOTES)
    {
        return splitter_handle_double_quotes(ctx, *str);
    }
    *end = 1;
    return NULL;
}

static void push_and_read(char *c, struct mbt_str **str,
                          struct splitter_ctx **ctx)
{
    mbt_str_pushc(*str, *c);
    stream_read((*ctx)->stream);
}

static struct shard *set_while_statement(int **while_statement, int set)
{
    **while_statement = set;
    return NULL;
}

static struct shard *shard_init_cas_3(struct mbt_str ***str)
{
    return shard_init(**str, false,
                      shard_is_redir(**str) ? SHARD_REDIR : SHARD_OPERATOR,
                      SHARD_UNQUOTED);
}

static struct shard *cas_1_to_5(char *c, struct splitter_ctx **ctx,
                                struct mbt_str **str, int *while_statement)
{
    *while_statement = 0;
    if (shard_is_any_operator(*str) || shard_is_redir(*str))
    {
        mbt_str_pushc(*str, *c);
        if (shard_is_any_operator(*str) || shard_is_redir(*str)) // Case 2
        {
            stream_read((*ctx)->stream);
            return set_while_statement(&while_statement, CONTINUE);
        }
        else // Case 3
        {
            mbt_str_pop(*str); // Not an operator -> We delimit

            return shard_init_cas_3(&str);
        }
    }

    if (strchr("\\\"\'", *c))
    {
        if ((*str)->size > 0)
        {
            return shard_init(*str, true, SHARD_WORD, SHARD_UNQUOTED);
        }

        if (*c == '\\')
        {
            splitter_handle_backslash(*ctx, *str);
            return shard_init(*str, true, SHARD_WORD, SHARD_BACKSLASH_QUOTED);
        }

        if (*c == '\'')
        {
            stream_read((*ctx)->stream);
            splitter_read_until(*ctx, *str, '\'');
            char end_quote = stream_read((*ctx)->stream);

            if (end_quote != '\'')
            {
                warnx("Syntax error: unmatched simple quote");
                (*ctx)->err = true;
                mbt_str_free(*str);
                return NULL;
            }

            if (!(*str)->size)
            {
                return set_while_statement(&while_statement, CONTINUE);
            }

            return shard_init(*str, true, SHARD_WORD, SHARD_SINGLE_QUOTED);
        }

        if (*c == '"')
        {
            stream_read((*ctx)->stream); // Pop the quote
            splitter_ctx_expect(*ctx, SHARD_CONTEXT_DOUBLE_QUOTES);
            return splitter_handle_double_quotes(*ctx, *str);
        }

        return set_while_statement(&while_statement, BREAK);
    }

    if (*c == '`' || *c == '$')
    {
        if ((*str)->size > 0)
        {
            return shard_init(*str, true, SHARD_WORD, SHARD_UNQUOTED);
        }

        if (*c == '$')
        {
            stream_read((*ctx)->stream);
        }

        return splitter_handle_expansion(*ctx, *str, false);
    }
    return set_while_statement(&while_statement, -1);
}

static struct shard *cas_6_to_11(char *c, struct splitter_ctx **ctx,
                                 struct mbt_str **str, int *while_statement)
{
    if (list_any_begins_with(OPERATORS, *c)) // Case 6: matched
    {
        if ((*str)->size > 0)
        {
            *while_statement = 0;
            return shard_init(*str, true, SHARD_WORD, SHARD_UNQUOTED);
        }

        mbt_str_pushc(*str, stream_read((*ctx)->stream));
        return set_while_statement(&while_statement, CONTINUE);
    }

    if (list_any_begins_with(REDIRS, *c))
    {
        // We allow at most 1 digit before redirection
        if ((*str)->size > 1 || ((*str)->size && !isdigit((*str)->data[0])))
        {
            *while_statement = BREAK;
            return NULL;
        }

        push_and_read(c, str, ctx);
        return set_while_statement(&while_statement, CONTINUE);
    }

    // Case 7: Newlines
    if (*c == '\n')
    {
        if ((*str)->size > 0)
        {
            *while_statement = 0;
            return shard_init(*str, true, SHARD_WORD, SHARD_UNQUOTED);
        }
        else
        {
            push_and_read(c, str, ctx);
        }

        *while_statement = 0;
        return shard_init(*str, false, SHARD_WORD, SHARD_UNQUOTED);
    }

    // Case 8: delimiter
    if (isspace(*c))
    {
        if ((*str)->size > 0)
        {
            *while_statement = 0;
            return shard_init(*str, true, SHARD_WORD, SHARD_UNQUOTED);
        }

        while ((*c = stream_peek((*ctx)->stream)) > 0 && isspace(*c))
        {
            stream_read((*ctx)->stream);
        }
        *while_statement = 0;

        return shard_init(*str, false, SHARD_DELIMITER, SHARD_UNQUOTED);
    }

    // Case 9: existing word
    if ((*str)->size > 0)
    {
        mbt_str_pushc(*str, stream_read((*ctx)->stream));
        *while_statement = CONTINUE;
        return NULL;
    }

    // Case 10: comments
    if (*c == '#')
    {
        discard_comment(*ctx);
        *while_statement = CONTINUE;
        return NULL;
    }

    // Case 11: new word: keep looping
    push_and_read(c, str, ctx);
    *while_statement = CONTINUE;
    return NULL;
}

static struct shard *splitter_next(struct splitter_ctx *ctx)
{
    struct mbt_str *str = mbt_str_init(64);

    if (ctx->expect->size)
    {
        int end = 0;
        struct shard *shard = ctx_expect(ctx, &end, &str);
        if (end == 0)
        {
            return shard;
        }
    }
    else
    {
        char c;
        int while_statement = 0;
        while ((c = stream_peek(ctx->stream)) > 0)
        {
            struct shard *shard2 = cas_1_to_5(&c, &ctx, &str, &while_statement);
            if (while_statement == CONTINUE)
            {
                continue;
            }
            else if (while_statement == BREAK)
            {
                break;
            }
            else if (while_statement == 0)
            {
                return shard2;
            }
            struct shard *shard = cas_6_to_11(&c, &ctx, &str, &while_statement);
            if (while_statement == CONTINUE)
            {
                continue;
            }
            else if (while_statement == BREAK)
            {
                break;
            }
            return shard;
        }
    }

    if (NOT_EMPTY(str))
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

    mbt_str_free(str);
    return NULL;
}

struct shard *splitter_peek(struct splitter_ctx *ctx)
{
    if (!ctx->cache)
    {
        ctx->cache = splitter_next(ctx);
    }

    return ctx->cache;
}

struct shard *splitter_pop(struct splitter_ctx *ctx)
{
    struct shard *shard = splitter_peek(ctx);
    ctx->cache = NULL;
    return shard;
}

static struct shard *case_parenthesis(struct splitter_ctx **ctx,
                                      struct mbt_str **str, struct case_p cp,
                                      enum shard_quote_type type)
{
    if (!cp.popped)
    {
        stream_read((*ctx)->stream);
    }

    while ((*(cp.c) = stream_peek((*ctx)->stream)) > 0 && *(cp.c) == ' ')
    {
        stream_read((*ctx)->stream);
    }

    bool is_arith = *(cp.c) == '(';
    if (is_arith)
    {
        stream_read((*ctx)->stream);
    }

    if (splitter_read_between(*ctx, *str, '(', ')'))
    {
        warnx("Syntax error: unmatched parenthesis");
        goto error;
    }

    if (!cp.popped)
    {
        stream_read((*ctx)->stream);
    }

    if (is_arith)
    {
        *(cp.c) = stream_read((*ctx)->stream);

        if (*(cp.c) != ')')
        {
            warnx("Syntax error: unmatched parenthesis");
            goto error;
        }
    }

    return shard_init(
        *str, true, is_arith ? SHARD_EXPANSION_ARITH : SHARD_EXPANSION_SUBSHELL,
        type);
error:
    (*ctx)->err = true;
    mbt_str_free(*str);
    return NULL;
}

static struct shard *else_handle_expansion(struct splitter_ctx **ctx,
                                           struct mbt_str **str, char *c,
                                           enum shard_quote_type type)
{
    while ((*c = stream_peek((*ctx)->stream)) > 0)
    {
        mbt_str_pushc(*str, *c);
        for (size_t i = 0; VARIABLES[i]; i++)
        {
            if (strcmp((*str)->data, VARIABLES[i]) == 0)
            {
                stream_read((*ctx)->stream);
                return shard_init(*str, true, SHARD_EXPANSION_VARIABLE, type);
            }
        }

        if (!is_valid_identifier(*str))
        {
            mbt_str_pop(*str);
            if (!(*str)->size)
            {
                break;
            }

            return shard_init(*str, true, SHARD_EXPANSION_VARIABLE, type);
        }

        stream_read((*ctx)->stream);
    }

    if (!(*str)->size)
    {
        mbt_str_pushc(*str, '$');
        return shard_init(*str, true, SHARD_WORD, type);
    }

    return shard_init(*str, true, SHARD_EXPANSION_VARIABLE, type);
}

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
            warnx("Syntax error: unmatched bracked");
            goto error;
        }

        stream_read(ctx->stream);
        return shard_init(str, true, SHARD_EXPANSION_VARIABLE, type);
    }
    else if (c == '(' || popped)
    {
        struct case_p cp = { popped, &c };
        return case_parenthesis(&ctx, &str, cp, type);
    }
    else if (c == '`')
    {
        stream_read(ctx->stream);
        if (splitter_read_until(ctx, str, '`'))
        {
            warnx("Syntax error: unmatched backtick");
            goto error;
        }

        stream_read(ctx->stream);
        return shard_init(str, true, SHARD_EXPANSION_SUBSHELL, type);
    }
    else
    {
        return else_handle_expansion(&ctx, &str, &c, type);
    }

error:
    ctx->err = true;
    mbt_str_free(str);
    return NULL;
}

static struct shard *long_return(struct mbt_str **str, char *c)
{
    return shard_init(*str, true,
                      *c == '*' ? SHARD_GLOBBING_STAR
                                : SHARD_GLOBBING_QUESTIONMARK,
                      SHARD_DOUBLE_QUOTED);
}

static struct shard *read_free_init(struct mbt_str **str,
                                    struct splitter_ctx **ctx)
{
    stream_read((*ctx)->stream);
    free(stack_pop((*ctx)->expect));
    return shard_init(*str, true, SHARD_WORD, SHARD_DOUBLE_QUOTED);
}

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
            return read_free_init(&str, &ctx);

        case '$':
            if (NOT_EMPTY(str)) // If there is already a token, return it
            {
                return shard_init(str, true, SHARD_WORD, SHARD_DOUBLE_QUOTED);
            }

            stream_read(ctx->stream); // Discard $
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
            return long_return(&str, &c);

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
            mbt_str_pushc(str, c);
            stream_read(ctx->stream);
            break;
        }
    }

error:
    mbt_str_free(str);
    warnx("Syntax error: unmatched quote");
    ctx->err = true;
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
