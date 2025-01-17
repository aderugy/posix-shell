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

struct ast_eval_ctx *ctx_init(void)
{
    struct ast_eval_ctx *ctx = calloc(1, sizeof(struct ast_eval_ctx));
    if (!ctx)
    {
        errx(EXIT_FAILURE, "ctx_init: not enough memory");
    }

    ctx->value = hash_map_init(64);
    ctx->check_redir = false;
    return ctx;
}

void ast_eval_ctx_free(struct ast_eval_ctx *ctx)
{
    if (ctx)
    {
        hash_map_free(ctx->value);
        free(ctx);
    }
}

struct mbt_str *env_vars(char *name)
{
    struct mbt_str *value = NULL;

    if (strcmp("OLDPWD", name) == 0)
    {
        value = mbt_str_init(8);
        mbt_str_pushcstr(value, getenv(name));
    }
    else if (strcmp("RANDOM", name) == 0)
    {
        value = mbt_str_init(8);
        int rand = 49;
        char *buffer = calloc(3, sizeof(char));
        mbt_str_pushcstr(value, my_itoa(rand, buffer));
        free(buffer);
    }
    else if (strcmp("PWD", name) == 0)
    {
        value = mbt_str_init(8);
        mbt_str_pushcstr(value, getenv(name));
    }
    else if (strcmp("IFS", name) == 0)
    {
        value = mbt_str_init(8);
        mbt_str_pushcstr(value, getenv(name));
    }
    else if (strcmp("UID", name) == 0)
    {
        value = mbt_str_init(8);
        uid_t uid = getuid();
        char *buffer = calloc(32, sizeof(char));
        mbt_str_pushcstr(value, my_itoa(uid, buffer));
        free(buffer);
    }

    return value;
}
struct mbt_str *get(struct ast_eval_ctx *ctx, struct mbt_str *name)
{
    // env vars
    struct mbt_str *value = env_vars(name->data);

    // local vars
    if (!value)
    {
        value = hash_map_get(ctx->value, name->data);
    }

    if (value)
    {
        logger("Retreived the value of %s : %s\n", name->data, value->data);
    }

    mbt_str_free(name);

    return value;
}

void insert(struct ast_eval_ctx *ctx, struct token *token)
{
    struct mbt_str *expanded = expand(ctx, token);

    char *data = expanded->data;
    char *eq = data;
    while (*eq != '=')
    {
        ++eq;
    }

    char *name = strndup(expanded->data, eq - data);
    logger("eval_ctx.c : found name : %s\n", name);
    logger("eval_ctx.c : found expansion : %s\n", expanded);

    // Basically the value after '='
    // this will be freed by the hashmap
    struct mbt_str *value = mbt_str_init(32);
    mbt_str_pushcstr(value, ++eq);
    mbt_str_free(expanded);

    hash_map_insert(ctx->value, name, (void *)value);
}
