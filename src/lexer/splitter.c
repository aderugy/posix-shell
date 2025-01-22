#include "splitter.h"

#include <ctype.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "mbtstr/str.h"
#include "shard.h"
#include "utils/logger.h"
#include "utils/xalloc.c"

DEFINE_VARIABLES;
DEFINE_REDIRS;
DEFINE_OPERATORS;

/*
 * ---------------------------------------------------------------------------
 * PROTOTYPES
 * ---------------------------------------------------------------------------
 */
static struct shard *splitter_handle_double_quotes(struct splitter_ctx *ctx,
                                                   struct mbt_str *str);

static struct shard *splitter_handle_expansion(struct splitter_ctx *ctx,
                                               struct mbt_str *str);

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
static struct shard *splitter_next(struct splitter_ctx *ctx)
{
    struct mbt_str *str = mbt_str_init(64);

    if (ctx->expect->size)
    {
        struct splitter_ctx_exp *exp = stack_peek(ctx->expect);

        if (exp->value == SHARD_CONTEXT_EXPANSION)
        {
            stack_pop(ctx->expect);
            return splitter_handle_expansion(ctx, str);
        }
        else if (exp->value == SHARD_CONTEXT_DOUBLE_QUOTES)
        {
            return splitter_handle_double_quotes(ctx, str);
        }
    }
    else
    {
        char c;
        while ((c = stream_peek(ctx->stream)) > 0)
        {
            // Case 1: EOF handled by exiting the loop
            if (shard_is_operator(str) || shard_is_redir(str))
            {
                mbt_str_pushc(str, c);
                if (shard_is_operator(str) || shard_is_redir(str)) // Case 2
                {
                    stream_read(ctx->stream);
                    continue;
                }
                else // Case 3
                {
                    mbt_str_pop(str); // Not an operator -> We delimit

                    return shard_init(str, false,
                                      shard_is_redir(str) ? SHARD_REDIR
                                                          : SHARD_OPERATOR,
                                      SHARD_UNQUOTED);
                }
            }

            if (strchr("\\\"\'", c))
            {
                if (NOT_EMPTY(str))
                {
                    return shard_init(str, true, SHARD_WORD, SHARD_UNQUOTED);
                }

                if (c == '\\')
                {
                    splitter_handle_backslash(ctx, str);
                    return shard_init(str, true, SHARD_WORD,
                                      SHARD_BACKSLASH_QUOTED);
                }

                if (c == '\'')
                {
                    stream_read(ctx->stream);
                    splitter_read_until(ctx, str, '\'');
                    stream_read(ctx->stream);

                    if (!str->size)
                    {
                        continue;
                    }

                    return shard_init(str, true, SHARD_WORD,
                                      SHARD_SINGLE_QUOTED);
                }

                if (c == '"')
                {
                    stream_read(ctx->stream); // Pop the quote
                    splitter_ctx_expect(ctx, SHARD_CONTEXT_DOUBLE_QUOTES);
                    return splitter_handle_double_quotes(ctx, str);
                }

                break;
            }

            if (c == '`' || c == '$' || c == '(')
            {
                if (NOT_EMPTY(str))
                {
                    return shard_init(str, true, SHARD_WORD, SHARD_UNQUOTED);
                }

                if (c == '$')
                {
                    stream_read(ctx->stream);
                }

                return splitter_handle_expansion(ctx, str);
            }

            // Case 5: Expansions
            // Case 6: New operator
            if (list_any_begins_with(OPERATORS, c)) // Case 6: matched
            {
                if (NOT_EMPTY(str))
                {
                    return shard_init(str, true, SHARD_WORD, SHARD_UNQUOTED);
                }

                mbt_str_pushc(str, stream_read(ctx->stream));
                continue;
            }

            if (list_any_begins_with(REDIRS, c))
            {
                // We allow at most 1 digit before redirection
                if (str->size > 1 || (str->size && !isdigit(str->data[0])))
                {
                    break;
                }

                mbt_str_pushc(str, c);
                stream_read(ctx->stream);
                continue;
            }

            // Case 7: Newlines
            if (c == '\n')
            {
                if (NOT_EMPTY(str))
                {
                    return shard_init(str, true, SHARD_WORD, SHARD_UNQUOTED);
                }
                else
                {
                    mbt_str_pushc(str, c);
                    stream_read(ctx->stream);
                }

                return shard_init(str, false, SHARD_WORD, SHARD_UNQUOTED);
            }

            // Case 8: delimiter
            if (isspace(c))
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

            // Case 9: existing word
            if (NOT_EMPTY(str))
            {
                mbt_str_pushc(str, stream_read(ctx->stream));
                continue;
            }

            // Case 10: comments
            if (c == '#')
            {
                discard_comment(ctx);
                continue;
            }

            // Case 11: new word: keep looping
            mbt_str_pushc(str, c);
            stream_read(ctx->stream);
        }
    }

    if (NOT_EMPTY(str))
    {
        if (shard_is_operator(str))
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

// @TIDY
static struct shard *splitter_handle_expansion(struct splitter_ctx *ctx,
                                               struct mbt_str *str)
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
    else if (c == '(')
    {
        stream_read(ctx->stream);
        c = stream_peek(ctx->stream);

        bool is_arith = c == '(';
        if (is_arith)
        {
            stream_read(ctx->stream);
        }

        if (splitter_read_between(ctx, str, '(', ')'))
        {
            warnx("Syntax error: unmatched parenthesis");
            goto error;
        }

        if (is_arith)
        {
            c = stream_read(ctx->stream);

            if (c != ')')
            {
                warnx("Syntax error: unmatched parenthesis");
                goto error;
            }
        }

        return shard_init(
            str, true,
            is_arith ? SHARD_EXPANSION_ARITH : SHARD_EXPANSION_SUBSHELL, type);
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
        while ((c = stream_peek(ctx->stream)) > 0)
        {
            mbt_str_pushc(str, c);
            for (size_t i = 0; VARIABLES[i]; i++)
            {
                if (strcmp(str->data, VARIABLES[i]) == 0)
                {
                    stream_read(ctx->stream);
                    return shard_init(str, true, SHARD_EXPANSION_VARIABLE,
                                      type);
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

error:
    ctx->err = true;
    mbt_str_free(str);
    return NULL;
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

            stream_read(ctx->stream);
            free(stack_pop(ctx->expect));
            return shard_init(str, true, SHARD_WORD, SHARD_DOUBLE_QUOTED);

        case '$':
            if (NOT_EMPTY(str)) // If there is already a token, return it
            {
                return shard_init(str, true, SHARD_WORD, SHARD_DOUBLE_QUOTED);
            }

            stream_read(ctx->stream); // Discard $
            return splitter_handle_expansion(ctx, str);

        case '`':
            return splitter_handle_expansion(ctx, str);

        case '?':
        case '*':
            if (NOT_EMPTY(str))
            {
                return shard_init(str, true, SHARD_WORD, SHARD_DOUBLE_QUOTED);
            }

            mbt_str_pushc(str, stream_read(ctx->stream));
            return shard_init(str, true,
                              c == '*' ? SHARD_GLOBBING_STAR
                                       : SHARD_GLOBBING_QUESTIONMARK,
                              SHARD_DOUBLE_QUOTED);

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
    if (c > 0)
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
    while ((c = stream_read(ctx->stream)) > 0)
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
