#ifndef VARS_H
#define VARS_H

#include "ast/nodes/eval_ctx.h"
#include "utils/my_itoa.h"

void init_hashtag(int nb_arg, struct ast_eval_ctx *ctx);
int init_args(int argc, char *argv[], struct ast_eval_ctx *ctx);
void init_dollar(struct ast_eval_ctx *ctx);
void update_qm(struct ast_eval_ctx *ctx, int ret_val);
void init_UID(struct ast_eval_ctx *ctx);

#endif // !VARS_H
