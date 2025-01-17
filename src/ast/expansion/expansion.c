#define _POSIX_C_SOURCE 200809L

#include "expansion.h"

#include <ctype.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "nodes/eval_ctx.h"
#include "nodes/node.h"
#include "utils/logger.h"
#include "utils/naming.h"

// retrieves the longest valid name from a '$'
// called upon no brackets behind the '$'
struct mbt_str *expand_dollar(struct ast_eval_ctx *ctx, struct dstream *dstream)
{
    // will be freed in a call to 'get'
    struct mbt_str *name = mbt_str_init(8);
    int c = dstream_read(dstream);

    // $0, $1, etc, $n, $@, $*, $#, etc
    if (isdigit(c) || strchr("@*#?$", c))
    {
        mbt_str_pushc(name, c);
        return get(ctx, name);
    }

    mbt_str_pushc(name, c);
    c = dstream_peek(dstream);

    // alphanumeric or '_'
    while (c && (isalnum(c) || c == '_'))
    {
        mbt_str_pushc(name, dstream_read(dstream));
        c = dstream_peek(dstream);
    }

    // no special parameter
    return get(ctx, name);
}

struct mbt_str *expand_brackets(struct ast_eval_ctx *ctx,
                                struct dstream *dstream, int *brackets)
{
    struct mbt_str *name = mbt_str_init(64);
    char c;

    // TODO
    while ((c = dstream_read(dstream)) && (c != '}' && dollar_valid(c)))
    {
        while (strchr("$\0}", c) == NULL)
        {
            mbt_str_pushc(name, c);
            c = dstream_read(dstream);
        }

        switch (c)
        {
            /*
            case '$' && dollar_valid(dstream_peek_state(dstream)):
                break;
            case '}' && dollar_valid(dstream_peek_state(dstream)):
                break;
            case '\0':
                break;
            default:
                mbt_str_pushc(name, c);
                break;
                */
        }
    }

    if (!c && *brackets > 0)
    {
        errx(EXIT_FAILURE, "expand_brackets: unclosed bracket left");
    }

    return get(ctx, name);
}

struct mbt_str *expand(struct ast_eval_ctx *ctx, struct token *token)
{
    struct dstream *dstream = dstream_from_str(token->value.c, token->state);
    struct mbt_str *str = mbt_str_init(64);
    int brackets = 0;
    char c;

    while ((c = dstream_read(dstream)))
    {
        while (strchr("$\0", c) == NULL)
        {
            mbt_str_pushc(str, c);
            c = dstream_read(dstream);
        }

        // case '$'
        if (c)
        {
            // If '$' single quoted or escaped
            if (!dollar_valid(dstream_peek_state(dstream)))
            {
                // Reads the '$' and pushes it
                mbt_str_pushc(str, dstream_read(dstream));
                continue;
            }

            c = dstream_peek(dstream);
            // Just a chill $A, $_JJJ, $ABBB, $1, $$, etc
            if (regular(c))
            {
                mbt_str_merge(str, expand_dollar(ctx, dstream));
            }
            // ${ etc } AND { is not escaped nor in single_quote
            else if (c == '{' && dollar_valid(dstream_peek_state(dstream)))
            {
                dstream_read(dstream);
                ++brackets;
                mbt_str_merge(str, expand_brackets(ctx, dstream, &brackets));
            }
            else
            {
                mbt_str_pushc(str, '$');
            }
        }
    }

    if (brackets > 0)
    {
        errx(EXIT_FAILURE, "expansion: unclosed brackets");
    }
    dstream_close(dstream);

    return str;
}
