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

// retrieves the longest valid name from a '$' or a ${
struct mbt_str *expand_dollar(struct ast_eval_ctx *ctx, struct dstream *dstream,
                              int bracket)
{
    // will be freed in a call to 'get'
    struct mbt_str *name = mbt_str_init(8);
    int c = dstream_read(dstream);

    // ${ NB }
    if (bracket && isdigit(c))
    {
        while (isdigit(c))
        {
            mbt_str_pushc(name, dstream_read(dstream));
            c = dstream_peek(dstream);
        }

        if (c != '}')
        {
            errx(EXIT_FAILURE, "bad substitution");
        }
    }

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

    // Unconventional character in substituion
    if (c && strchr("$}", c) == NULL)
    {
        errx(EXIT_FAILURE, "bad substitution");
    }

    if (bracket && c != '}')
    {
        errx(EXIT_FAILURE, "expand_dollar: unclosed left bracket");
    }

    // no special parameter
    return get(ctx, name);
}

struct mbt_str *expand(struct ast_eval_ctx *ctx, struct token *token)
{
    struct dstream *dstream = dstream_from_str(token->value.c, token->state);
    struct mbt_str *str = mbt_str_init(64);
    char c;

    while ((c = dstream_read(dstream)))
    {
        while (strchr("$\0", c) == NULL)
        {
            mbt_str_pushc(str, c);
            c = dstream_read(dstream);
        }

        int valid = dollar_valid(dstream_peek_state(dstream));

        // case '$'
        if (c == '$')
        {
            // If '$' double quoted or unescaped
            if (valid)
            {
                c = dstream_peek(dstream);
                valid = dollar_valid(dstream_peek_state(dstream));

                // Case $REGULAR_NAME OR special parameter
                // Just a chill $A, $1, $$, $A_B,  etc
                if (regular(c))
                {
                    mbt_str_merge(str, expand_dollar(ctx, dstream, 0));
                }

                // ${ } AND { is not escaped nor in single_quote
                else if (c == '{' && valid)
                {
                    dstream_read(dstream);
                    mbt_str_merge(str, expand_dollar(ctx, dstream, 1));
                    dstream_read(dstream);
                }
                else
                {
                    mbt_str_pushc(str, '$');
                }
            }
            else
            {
                mbt_str_pushc(str, '$');
            }
        }
    }

    dstream_close(dstream);
    return str;
}
