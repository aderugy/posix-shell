#define _POSIX_C_SOURCE 200809L

#include "splitter.h"

#include <ctype.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbtstr/str.h"
#include "utils/logger.h"
#include "utils/xalloc.c"

#define IS_REDIR_FALSE 0
#define IS_REDIR_PARTIAL 1
#define IS_REDIR_MATCH 2

#define NOT_EMPTY(str) str->size > 0

static const char *OPERATORS[] = { ";", "&&", "&", "|", "||", NULL };

static const char *REDIRS[] = { ">>", ">&", "<&", "<>", ">|",
                                "|>", ">",  "<",  NULL };

static struct shard *shard_init(char *data, bool can_chain, int shard_type);
static void splitter_handle_backslash(struct stream *stream,
                                      struct mbt_str *str);
static void splitter_handle_single_quotes(struct stream *stream,
                                          struct mbt_str *str);
static bool list_any_begins_with(const char *list[], char c);
static int shard_is_redir(struct mbt_str *str);
static int shard_is_operator(struct mbt_str *str);
static void discard_comment(struct stream *stream);
static int splitter_handle_expansion(struct stream *stream,
                                     struct mbt_str *str);

void shard_print(struct shard *shard)
{
    if (!shard)
    {
        logger("(nil)\n");
        return;
    }

    logger("%s%s\n", shard->data, shard->can_chain ? " (*)" : "");
}

void shard_free(struct shard *shard)
{
    free(shard->data);
    free(shard);
}

struct splitter_ctx *splitter_ctx_init(void)
{
    struct splitter_ctx *ctx = xcalloc(1, sizeof(struct splitter_ctx));
    ctx->expect = stack_init(free);
    return ctx;
}

void splitter_ctx_expect(struct splitter_ctx *ctx, int value)
{
    struct splitter_ctx_exp *exp = xcalloc(1, sizeof(struct splitter_ctx_exp));
    exp->value = value;
    stack_push(ctx->expect, exp);
}

struct shard *splitter_next(struct stream *stream, struct splitter_ctx *ctx)
{
    int shard_type = 0;
    bool can_chain = false;
    struct mbt_str *str = mbt_str_init(64);

    if (ctx->expect->size)
    {
        // L'idée est de pouvoir jump à une certaine partie du splitter selon le
        // contexte. Par exemple: si je m'attends a une double quote, je
        // reprends mon parsing comme avant d'avoir été interrompu par un
        // subshell / globbing ou autre
        struct splitter_ctx_exp *exp = stack_pop(ctx->expect);
        if (exp->value == SHARD_CONTEXT_EXPANSION)
        {
            shard_type = splitter_handle_expansion(stream, str);
        }

        if (exp->value == SHARD_CONTEXT_DOUBLE_QUOTES)
        {
            errx(EXIT_FAILURE, "not implemented");
        }

        free(exp);
    }
    else
    {
        char c;
        while ((c = stream_peek(stream)) != EOF)
        {
            // Case 1: EOF handled by exiting the loop
            if (shard_is_operator(str) || shard_is_redir(str))
            {
                mbt_str_pushc(str, c);
                if (shard_is_operator(str) || shard_is_redir(str)) // Case 2
                {
                    stream_read(stream);
                    continue;
                }
                else // Case 3
                {
                    mbt_str_pop(str); // Not an operator -> We delimit
                    can_chain = false;
                    break;
                }
            }

            /*
             * Devrait simplement initier le processus de lexing du texte quoté
             * Doit être reversible, IE si je call handle quotes dans cette fn,
             * meme résultat que si je call handle quotes en ayant pop ma stack
             * des expectations
             */
            if (strchr("\\\"\'", c))
            {
                if (NOT_EMPTY(str))
                {
                    can_chain = true;
                    break;
                }

                if (c == '\\')
                {
                    splitter_handle_backslash(stream, str);
                    can_chain = true;
                    break;
                }

                if (c == '\'')
                {
                    splitter_handle_single_quotes(stream, str);
                    can_chain = true;
                    break;
                }

                if (c == '"')
                {
                    if (NOT_EMPTY(str))
                    {
                        can_chain = true;
                        break;
                    }

                    stream_read(stream); // Pop the quote
                    splitter_ctx_expect(ctx, SHARD_CONTEXT_DOUBLE_QUOTES);
                    shard_type = SHARD_DOUBLE_QUOTED;

                    while ((c = stream_read(stream)) > 0)
                    {
                        if (c == '$')
                        {
                            if (NOT_EMPTY(str))
                            {
                                splitter_ctx_expect(ctx,
                                                    SHARD_CONTEXT_EXPANSION);
                                break;
                            }

                            splitter_handle_expansion(stream, str);
                        }
                        else if (c == '"')
                        {
                            break;
                        }
                        else if (c == '\\')
                        {
                            splitter_handle_backslash(stream, str);
                        }
                        else
                        {
                            mbt_str_pushc(str, c);
                        }
                    }
                }

                break;
            }

            if (c == '$' || c == '`')
            {
                if (NOT_EMPTY(str))
                {
                    splitter_ctx_expect(ctx, SHARD_EXPANSION_SUBSHELL);
                    break;
                }

                // read a subshell
                errx(EXIT_FAILURE, "not implemented");
            }

            // Case 5: Expansions
            // Case 6: New operator
            bool is_op = list_any_begins_with(OPERATORS, c);
            if (is_op) // Case 6: matched
            {
                if (str->size)
                {
                    can_chain = false;
                    break;
                }

                mbt_str_pushc(str, c);
                stream_read(stream);
                continue;
            }

            bool is_rdir = list_any_begins_with(REDIRS, c);
            if (is_rdir)
            {
                if (str->size > 1 || (str->size && !isdigit(str->data[0])))
                {
                    break;
                }

                mbt_str_pushc(str, c);
                stream_read(stream);
                continue;
            }

            // Case 7: Newlines
            if (c == '\n')
            {
                if (!str->size)
                {
                    mbt_str_pushc(str, c);
                    stream_read(stream);
                }

                can_chain = false;
                break;
            }

            // Case 8: delimiter
            if (isspace(c))
            {
                stream_read(stream);

                if (str->size)
                {
                    can_chain = false;
                    break;
                }
                else
                {
                    continue;
                }
            }

            // Case 9: existing word
            if (str->size)
            {
                mbt_str_pushc(str, c);
                stream_read(stream);
                continue;
            }

            // Case 10: comments
            if (c == '#')
            {
                discard_comment(stream);
                continue;
            }

            // Case 11: new word: keep looping
            mbt_str_pushc(str, c);
            stream_read(stream);
            continue;
        }
    }

    char *data = NULL;
    if (str->size)
    {
        data = strdup(str->data);
    }
    mbt_str_free(str);

    if (data)
    {
        return shard_init(data, can_chain, shard_type);
    }

    return NULL;
}

static struct shard *shard_init(char *data, bool can_chain, int shard_type)
{
    struct shard *shard = xcalloc(1, sizeof(struct shard));
    shard->data = data;
    shard->can_chain = can_chain;
    shard->shard_type = shard_type;
    return shard;
}



static int splitter_handle_expansion(struct stream *stream, struct mbt_str *str)
{
    char c = stream_peek(stream);
    if (c == '{')
    {
        stream_read(stream);
        while ((c = stream_read(stream)) > 0 && c != '}')
        {
            mbt_str_pushc(str, c);
        }

        return SHARD_EXPANSION_VARIABLE;
    }
    else if (c == '(')
    {
        stream_read(stream);
        c = stream_peek(stream);

        bool is_arith = c == '(';
        if (is_arith)
        {
            stream_read(stream);
        }

        int pcount = 0;
        while ((c = stream_read(stream)) > 0)
        {
            if (c == '(')
            {
                pcount++;
            }

            if (c == ')')
            {
                if (!pcount)
                {
                    break;
                }

                pcount--;
            }

            mbt_str_pushc(str, c);
        }

        if (is_arith)
        {
            c = stream_read(stream);

            if (c != ')')
            {
                errx(EXIT_FAILURE, "unmatched parenthesis");
            }

            return SHARD_EXPANSION_ARITH;
        }

        return SHARD_EXPANSION_SUBSHELL;
    }
    else
    {
        mbt_str_pushc(str, c);
        return SHARD_EXPANSION_VARIABLE;
    }
}

static void splitter_handle_backslash(struct stream *stream,
                                      struct mbt_str *str)
{
    stream_read(stream);

    char c = stream_read(stream);
    if (c > 0)
    {
        mbt_str_pushc(str, c);
    }
}

static void splitter_handle_single_quotes(struct stream *stream,
                                          struct mbt_str *str)
{
    stream_read(stream);

    char c;
    while ((c = stream_read(stream)) && c >= 0 && c != '\'')
    {
        mbt_str_pushc(str, c);
    }
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

static int shard_is_redir(struct mbt_str *str)
{
    int status = IS_REDIR_FALSE;

    if (NOT_EMPTY(str))
    {
        return status;
    }

    char *redir = str->data;
    if (isdigit(*redir))
    {
        redir++;
    }

    if (!*redir)
    {
        return status;
    }

    for (size_t i = 0; REDIRS[i]; i++)
    {
        int sstatus = IS_REDIR_FALSE;

        for (size_t j = 0;; j++)
        {
            char c1 = REDIRS[i][j];
            char c2 = redir[j];
            if (!c1 && !c2)
            {
                sstatus = IS_REDIR_MATCH;
                break;
            }
            else if (c1 && !c2)
            {
                sstatus = IS_REDIR_PARTIAL;
            }
            else if (c1 && c2)
            {
                if (c1 != c2)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }

        if (sstatus > status)
        {
            status = sstatus;
        }
    }

    return status;
}

static int shard_is_operator(struct mbt_str *str)
{
    for (size_t i = 0; OPERATORS[i]; i++)
    {
        if (strcmp(OPERATORS[i], str->data) == 0)
        {
            return 1;
        }
    }

    return 0;
}

static void discard_comment(struct stream *stream)
{
    char c;
    while ((c = stream_peek(stream)) != '\n' && c != 0 && c != EOF)
    {
        stream_read(stream); // Discard every char until \n 0 and -1
                             // (check with numeric value)
    }
}
