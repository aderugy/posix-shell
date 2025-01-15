#ifndef EVAL_CTX_H
#define EVAL_CTX_H

#include "lexer/lexer.h"
#include "utils/hash_map.h"

struct ast_eval_ctx
{
    struct hash_map *value;
};

struct ast_eval_ctx *ctx_init(void);
void ast_eval_ctx_free(struct ast_eval_ctx *ctx);

void insert(struct ast_eval_ctx *ctx, struct token *token);
#endif // !EVAL_CTX_H
