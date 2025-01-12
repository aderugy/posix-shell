#ifndef COMMAND_H
#define COMMAND_H

#include "lexer/lexer.h"
#include "node.h"
#include "simple_command.h"
#include "utils/linked_list.h"

enum command_type
{
    SHELL_CMD,
    SIMPLE_CMD
};

struct ast_cmd
{
    enum command_type type;
    struct ast_node *cmd;
    struct linked_list *redirs;
};

struct ast_cmd *ast_parse_cmd(struct lexer *lexer);
int ast_eval_cmd(struct ast_cmd *node, void **out);
void ast_free_cmd(struct ast_cmd *node);

void ast_print_cmd(struct ast_cmd *node);

#endif // !COMMAND_H
