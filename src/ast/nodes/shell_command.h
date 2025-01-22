#ifndef SHELL_COMMAND_H
#define SHELL_COMMAND_H

#include "lexer/lexer.h"
#include "node.h"

struct ast_shell_cmd
{
    struct ast_node *ast_node;
    char is_sub_shell;
};

struct ast_shell_cmd *ast_parse_shell_cmd(struct lexer *lexer);
int ast_eval_shell_cmd(struct ast_shell_cmd *cmd, void **ptr,
                       __attribute((unused)) struct ast_eval_ctx *ctx);
void ast_free_shell_cmd(struct ast_shell_cmd *cmd);
void ast_print_shell_cmd(struct ast_shell_cmd *cmd);

#endif // !SHELL_COMMAND_H
