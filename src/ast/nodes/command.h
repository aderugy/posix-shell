#ifndef COMMAND_H
#define COMMAND_H

#include "lexer/lexer.h"
#include "node.h"
#include "simple_command.h"

struct ast_cmd
{
    struct ast_node *simple_cmd;
};

struct ast_cmd *ast_parse_cmd(struct lexer *lexer);
int ast_eval_cmd(struct ast_cmd *node, void **out);
void ast_free_cmd(struct ast_cmd *node);

#endif // !COMMAND_H
