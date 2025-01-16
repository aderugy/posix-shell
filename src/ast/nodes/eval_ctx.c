#include "eval_ctx.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "utils/logger.h"

struct ast_eval_ctx *ctx_init(void)
{
    struct ast_eval_ctx *ctx = calloc(1, sizeof(struct ast_eval_ctx));
    if (!ctx)
    {
        errx(EXIT_FAILURE, "ctx_init: not enough memory");
    }

    ctx->value = hash_map_init(64);
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

struct mbt_str *get(struct ast_eval_ctx *ctx, struct mbt_str *name)
{
    struct mbt_str *value = hash_map_get(ctx->value, name->data);
    logger("Retreived the value of %s : %s\n", name->data, value->data);

    mbt_str_free(name);

    return value;
}

void insert(struct ast_eval_ctx *ctx, struct token *token)
{
    char *data = token->value.c;
    char *eq = data;
    while (*eq != '=')
    {
        ++eq;
    }

    char *name = strndup(data, eq - data);
    logger("eval_ctx.c : found name : %s\n", name);
    // TODO call EXPANSION
    void *expanded = strdup(++eq);
    logger("eval_ctx.c : found expension : %s\n", expanded);

    // errx(EXIT_FAILURE, "insert_ctx: not implemented");
    hash_map_insert(ctx->value, name, expanded);
    logger("Assigned %s=%s\n", name, expanded);
}
