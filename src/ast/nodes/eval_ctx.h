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

struct ast_eval_ctx *ctx_init(void);
void ctx_free(struct ast_eval_ctx *ctx);

struct ast_node *ctx_get_function(struct ast_eval_ctx *ctx, char *name);
char *ctx_get_variable(struct ast_eval_ctx *ctx, char *name);

int ctx_set_local_variable(struct ast_eval_ctx *ctx, char *name, char *str);
int ctx_set_function(struct ast_eval_ctx *ctx, char *name,
                     struct ast_node *function);

#endif // !EVAL_CTX_H
