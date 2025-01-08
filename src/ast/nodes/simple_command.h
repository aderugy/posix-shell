#ifndef AST_SIMPLE_COMMAND_H
#define AST_SIMPLE_COMMAND_H

#include "lexer/lexer.h";
#include "node.h"
#include "utils/linked_list.h"

struct ast_simple_cmd
{
    struct linked_list *args;
};

struct ast_simple_cmd *ast_parse_simple_cmd(struct lexer *lexer);
int ast_eval_simple_cmd(struct ast_simple_cmd *cmd, void **out);
void ast_free_simple_cmd(struct ast_simple_cmd *cmd);

void ast_print_simple_cmd(struct ast_simple_cmd *cmd);

#endif // !AST_SIMPLE_COMMAND_H
