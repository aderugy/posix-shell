#include "eval_ctx.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "expansion/expansion.h"
#include "node.h"
#include "utils/logger.h"
#include "utils/my_itoa.h"
#include "utils/xalloc.h"

// @RENAME

// @ANSWER
// rename the the way that pleases you, i can't guess it if you don't tell me
struct ast_eval_ctx *ast_eval_ctx_init(void)
{
    struct ast_eval_ctx *ctx = xcalloc(1, sizeof(struct ast_eval_ctx));
    ctx->value = hash_map_init(64);
    ctx->break_count = 0;
    ctx->continue_count = 0;
    return ctx;
}

void ast_eval_ctx_free(struct ast_eval_ctx *ctx)
{
    if (ctx)
    {
        if (ctx->value)
        {
            hash_map_free(ctx->value);
        }

        free(ctx);
    }
}

/*
 * // @RENAME
// @REFACTOR: n'est pas censé calloc la return value.

// @ANSWER
// rename the way that pleases you, i can't guess it if you don't tell me

 * struct mbt_str *env_vars(char *name)
{
    struct mbt_str *value = mbt_str_init(8);

    char *env_var;
    if ((env_var = getenv(name)) != NULL)
    {
        mbt_str_pushcstr(value, env_var);
    }
    else
    {
        mbt_str_free(value);
        value = NULL;
    }

    return value;
}*/

// @RENAME
// @REFACTOR: env_vars renvoie une valeur a free, et pas hash_map_get.

// @ANSWER
// rename the way that pleases you, i can't guess it if you don't tell me
// @DONE Both values must now be freed
//

char *ctx_get_value(struct ast_eval_ctx *ctx, struct mbt_str *name)
{
    // env vars
    char *value = NULL;
    if ((value = getenv(name->data)) == NULL)
    {
        struct mbt_str *tmp = hash_map_get(ctx->value, name->data);
        if (tmp)
        {
            value = tmp->data;
        }
    }

    return value;
    /* local vars
    if (value == NULL)
    {
        struct mbt_str *tmp = hash_map_get(ctx->value, name->data);
        if (tmp)
        {
            value = mbt_str_init(8);
            mbt_str_pushcstr(value, tmp->data);
        }
    }*/
}

// @RENAME
// @REFACTOR
// Pourquoi prend il un token ? Pourquoi il expand a l'insertion et pas à
// l'evaluation ?

// @ANSWER
// Token is used during expansion, cf line 81
// Bc the token has the information about the state of each characters
// cf. 'struct token'; field 'state'
//
// Expansion is actually done in EVAL, because insert is called during eval
// for example : in the function ast_eval_element
// this aims at removing redundant code in evaluation
// feel free to split it
int insert(struct ast_eval_ctx *ctx, struct token *token)
{
    struct mbt_str *expanded = expand(ctx, token);

    char *data = expanded->data;
    char *eq = strchr(data, '=');
    if (!eq)
    {
        warnx("insert: no '=' in token");
        return 1;
    }

    char *name = strndup(data, eq - data); // ok on re alloue
    // pour garder un pointeur convenable

    // Basically the value after '='
    // this will be freed by the hashmap
    struct mbt_str *value = mbt_str_init(32);
    mbt_str_pushcstr(value, ++eq);
    mbt_str_free(expanded);

    int return_value = hash_map_insert(ctx->value, name, (void *)value);

    free(name);

    return return_value;
}

/*
 * Updates the value for 'name' variable
 */
void ast_eval_ctx_set_local_var(struct ast_eval_ctx *ctx, char *name,
                                char *value)
{
    struct mbt_str *str = mbt_str_init(strlen(value) + 1);
    mbt_str_pushcstr(str, value);

    hash_map_insert(ctx->value, name, str);
}
