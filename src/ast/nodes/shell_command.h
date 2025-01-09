#ifndef AST_SHELL_COMMAND_H
#define AST_SHELL_COMMAND_H

#include "lexer/lexer.h"

struct ast_shell_cmd
{
    int cum;
};

struct ast_shell_cmd *ast_parse_shell_cmd(struct lexer *lexer);
int ast_eval_shell_cmd(struct ast_shell_cmd *cmd, void **ptr);
void ast_free_shell_cmd(struct ast_shell_cmd *cmd);
void ast_print_shell_cmd(struct ast_shell_cmd *cmd);

#endif // !AST_SHELL_COMMAND_H
