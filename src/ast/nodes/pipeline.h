#ifndef PIPELINE_H
#define PIPELINE_H

#include "lexer/lexer.h"
#include "node.h"
#include "utils/linked_list.h"

struct ast_pipeline
{
    struct linked_list *commands;
    char not ;
};

struct ast_pipeline *ast_parse_pipeline(struct lexer *lexer);
int ast_eval_pipeline(struct ast_pipeline *node, struct linked_list *out,
                      __attribute((unused)) struct ast_eval_ctx *ctx);
void ast_free_pipeline(struct ast_pipeline *pipeline);
void ast_print_pipeline(struct ast_pipeline *pipeline);

#endif // !PIPELINE_H
