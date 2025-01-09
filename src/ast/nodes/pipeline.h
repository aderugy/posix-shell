#ifndef AST_PIPELINE_H
#define AST_PIPELINE_H

#include "lexer/lexer.h"
#include "node.h"

struct ast_pipeline
{
    struct ast_node *command;
};

struct ast_pipeline *ast_parse_pipeline(struct lexer *lexer);
int ast_eval_pipeline(struct ast_pipeline *node, void **out);
void ast_free_pipeline(struct ast_pipeline *pipeline);
void ast_print_pipeline(struct ast_pipeline *pipeline);

#endif // !AST_PIPELINE_H
