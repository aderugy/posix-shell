#include "splitter.h"
#include "utils/xalloc.h"

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

void splitter_ctx_free(struct splitter_ctx *ctx)
{
    stack_free(ctx->expect);
    free(ctx);
}

enum shard_quote_type splitter_ctx_get_active_quote(struct splitter_ctx *ctx)
{
    struct stack *expected = ctx->expect;

    struct stack_elt *head = expected->head;
    while (head)
    {
        struct splitter_ctx_exp *el = head->data;
        if (el->value == SHARD_CONTEXT_DOUBLE_QUOTES)
        {
            return SHARD_DOUBLE_QUOTED;
        }

        head = head->next;
    }

    return SHARD_UNQUOTED;
}
