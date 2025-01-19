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
struct ast_eval_ctx *ctx_init(void)
{
    struct ast_eval_ctx *ctx = xcalloc(1, sizeof(struct ast_eval_ctx));
    ctx->value = hash_map_init(64);
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

// @RENAME
// @REFACTOR: n'est pas censé calloc la return value.
struct mbt_str *env_vars(char *name)
{
    struct mbt_str *value = mbt_str_init(8);

    if (strcmp("OLDPWD", name) == 0)
    {
        mbt_str_pushcstr(value, getenv(name));
    }
    else if (strcmp("RANDOM", name) == 0)
    {
        int rand = 49; // ??
        char buffer[3];
        mbt_str_pushcstr(value, my_itoa(rand, buffer));
    }
    else if (strcmp("PWD", name) == 0)
    {
        mbt_str_pushcstr(value, getenv(name));
    }
    else if (strcmp("IFS", name) == 0)
    {
        mbt_str_pushcstr(value, getenv(name));
    }
    else if (strcmp("UID", name) == 0)
    {
        uid_t uid = getuid();
        char buffer[32];
        mbt_str_pushcstr(value, my_itoa(uid, buffer));
    }
    else
    {
        mbt_str_free(value);
        value = NULL;
    }

    return value;
}

// @RENAME
// @REFACTOR: env_vars renvoie une valeur a free, et pas hash_map_get.
struct mbt_str *get(struct ast_eval_ctx *ctx, struct mbt_str *name)
{
    // env vars
    struct mbt_str *value = env_vars(name->data);

    // local vars
    if (value == NULL)
    {
        value = hash_map_get(ctx->value, name->data);
    }

    return value;
}

// @RENAME
// @REFACTOR
// Pourquoi prend il un token ? Pourquoi il expand a l'insertion et pas à
// l'evaluation ?
void insert(struct ast_eval_ctx *ctx, struct token *token)
{
    struct mbt_str *expanded = expand(ctx, token);

    char *data = expanded->data;
    char *eq = strchr(data, '=');
    if (!eq)
    {
        errx(EXIT_FAILURE, "insert: no '=' in token");
    }

    char *name = strndup(expanded->data, eq - data);

    // Basically the value after '='
    // this will be freed by the hashmap
    struct mbt_str *value = mbt_str_init(32);
    mbt_str_pushcstr(value, ++eq);
    mbt_str_free(expanded);

    hash_map_insert(ctx->value, name, (void *)value);
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
