#ifndef VARS_H
#define VARS_H

#include "ast/nodes/eval_ctx.h"
#include "utils/linked_list.h"
#include "utils/my_itoa.h"

void ctx_init_local_hashtag(int nb_arg, struct ast_eval_ctx *ctx);
void ctx_init_local_dollar(struct ast_eval_ctx *ctx);
void ctx_update_local_qm(struct ast_eval_ctx *ctx, int return_value);
void ctx_init_local_UID(struct ast_eval_ctx *ctx);

int ctx_init_local_args(int argc, char *argv[], struct ast_eval_ctx *ctx);

void ctx_restore_spe_vars(struct ast_eval_ctx *ctx,
                          struct linked_list *old_ctx);
struct linked_list *ctx_save_spe_vars(struct ast_eval_ctx *ctx);

#endif // !VARS_H
