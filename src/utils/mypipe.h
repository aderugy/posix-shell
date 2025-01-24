#ifndef MYPIPE_H
#define MYPIPE_H

#include "ast/nodes/eval_ctx.h"
#include "linked_list.h"

int exec_pipeline(struct linked_list *linked_list, struct ast_eval_ctx *ctx);

#endif /* ! MYPIPE_H */
