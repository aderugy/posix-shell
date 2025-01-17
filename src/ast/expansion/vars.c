#include "vars.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/splitter.h"

// inits the number of args
void init_hashtag(int nb_arg, struct ast_eval_ctx *ctx)
{
    // KEY
    struct mbt_str *key = mbt_str_init(8);
    mbt_str_pushc(key, '#');

    // VALUE
    struct mbt_str *value = mbt_str_init(8);
    char *buffer = calloc(64, sizeof(char));
    mbt_str_pushcstr(value, my_itoa(nb_arg, buffer));
    free(buffer);

    hash_map_insert(ctx->value, strdup(key->data), strdup(value->data));

    mbt_str_free(key);
    mbt_str_free(value);
}
// inits the number of args
int init_args(int argc, char *argv[], struct ast_eval_ctx *ctx)
{
    struct mbt_str *arobase = mbt_str_init(64);

    for (int i = 2; i < argc; i++)
    {
        struct mbt_str *str = mbt_str_init(8);
        mbt_str_pushcstr(str, argv[i]);

        //  FOR $@
        mbt_str_pushcstr(arobase, str->data);
        if (i < argc - 1)
        {
            mbt_str_pushc(arobase, ' ');
        }

        char *number = calloc(1, 65);
        hash_map_insert(ctx->value, my_itoa(i - 1, number), str);
    }

    char *karabose = calloc(1, 2);
    karabose[0] = '@';

    hash_map_insert(ctx->value, karabose, arobase);

    return 2 - argc;
}
