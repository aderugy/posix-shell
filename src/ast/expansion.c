#define _POSIX_C_SOURCE 200809L

#include "expansion.h"

#include <ctype.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "nodes/eval_ctx.h"
#include "nodes/node.h"
#include "utils/logger.h"

// retrieves the longest valid name from a '$'
struct mbt_str *expand_dollar(struct dstream *dstream, struct ast_eval_ctx *ctx)
{
    struct mbt_str *name = mbt_str_init(8);
    int c = dstream_peek(dstream);

    // $0, $1, etc, $n, $@, $*, $#, etc
    if (isdigit(c) || strchr("@*#?$", c))
    {
        mbt_str_pushc(name, c);
        return get(ctx, name);
    }

    while (c && (isalnum(c) || c == '_'))
    {
        mbt_str_pushc(name, c);
        c = dstream_read(dstream);
    }

    // no special parameter
    return name;
}

/*
//returns the value of a param if it exists
struct mbt_str *get_param(struct mbt_str *str)
{
    const char *name = strndup(str->data, str->size);

    char *value = getenv(name);
    free(name);

    if (!value)
    {
        return NULL;
    }

    struct mbt_str *val = mbt_str_init(64);
    mbt_str_pushcstr(value);

    return val;
}

void expand(struct mbt_str *str, struct dstream* dstream, int *brackets)
{
    char c;

    while ((c = dstream_peek(dstream)) != EOF)
    {
        while (strchr("$\0", (c = dstream_read(dstream))) == NULL)
        {
            mbt_str_pushc(str, c);
        }

        // If '$' single quoted or escaped
        if (!dollar_valid(dstream_peek_state(dstream)))
        {
            mbt_str_pushc(str, c);
            continue;
        }

        // case '$'
        if (c != EOF)
        {
            if (c == '{' && dollar_valid(dstream_peek_state(dstream)))
            {
                dstream_read(dstream);
                // TODO
            }
            else
            {
                // TODO
                c = dstream_read(dstream);
                expand(str, dstream, brackets);
            }
        }
    }
}

struct mbt_str *expand_word(struct token *token)
{
    struct dstream *dstream = dstream_init(token->value.c, token->state);
    struct mbt_str *str = mbt_str_init(64);
    int brackets = 0;

    expand(str, dstream, &brackets);

    if (brackets > 0)
    {
        errx(EXIT_FAILURE, "expansion: unclosed brackets")
    }

    return str;
}
*/
