#include "eval_ctx.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "node.h"
#include "utils/logger.h"
#include "utils/my_itoa.h"
#include "utils/xalloc.h"

struct ast_eval_ctx *ctx_init(void)
{
    struct ast_eval_ctx *ctx = xcalloc(1, sizeof(struct ast_eval_ctx));
    ctx->value = hash_map_init(64);
    ctx->break_count = 0;
    ctx->continue_count = 0;
    ctx->stream_type = STREAM_FROM_FILE;
    return ctx;
}

void ctx_free(struct ast_eval_ctx *ctx)
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

struct ast_node *ctx_get_function(struct ast_eval_ctx *ctx, char *name)
{
    return hash_map_get(ctx->value, name, HASH_FUNCTION);
}

char *ctx_get_variable(struct ast_eval_ctx *ctx, char *name)
{
    char *variable_value = getenv(name);
    if (variable_value == NULL)
    {
        variable_value = hash_map_get(ctx->value, name, HASH_VARIABLE);
    }
    return variable_value;
}

/*
 * Updates the value for 'name' local variable
 */
int ctx_set_local_variable(struct ast_eval_ctx *ctx, char *name, char *str)
{
    name = strdup(name);
    str = strdup(str);
    return hash_map_insert(ctx->value, name, (void *)str, HASH_VARIABLE);
}

/*
 * Updates the value for 'name' function
 */
int ctx_set_function(struct ast_eval_ctx *ctx, char *name,
                     struct ast_node *function)
{
    name = strdup(name);
    // TODO check when to free functions
    return hash_map_insert(ctx->value, name, (void *)function, HASH_FUNCTION);
}
