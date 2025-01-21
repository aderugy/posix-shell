#ifndef EVAL_CTX_H
#define EVAL_CTX_H

#include "lexer/lexer.h"
#include "utils/hash_map.h"

struct ast_eval_ctx
{
    struct hash_map *value;
    bool check_redir;
    long break_count;
    long continue_count;
};

struct ast_eval_ctx *ast_eval_ctx_init(void);
void ast_eval_ctx_free(struct ast_eval_ctx *ctx);

int ctx_insert_value(struct ast_eval_ctx *ctx, struct token *token);
char *ctx_get_value(struct ast_eval_ctx *ctx, struct mbt_str *name);

void ast_eval_ctx_set_local_var(struct ast_eval_ctx *ctx, char *name,
                                char *value);

#endif // !EVAL_CTX_H
