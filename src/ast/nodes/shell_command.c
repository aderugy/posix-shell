#include "shell_command.h"

#include <err.h>
#include <stdlib.h>

#include "node.h"

/*
 * shell_command = rule_if ;
 */
struct ast_shell_cmd *ast_parse_shell_cmd(struct lexer *lexer)
{
    struct ast_shell_cmd *node = calloc(1, sizeof(struct ast_shell_cmd));
    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    struct ast_node *if_rule = ast_create(lexer, AST_IF);
    if (!if_rule)
        return NULL;
    node->ast_node = if_rule;
    return node;
}
int ast_eval_shell_cmd(struct ast_shell_cmd *cmd, void **ptr)
{
    return ast_eval(cmd->ast_node, ptr);
}
void ast_free_shell_cmd(struct ast_shell_cmd *cmd)
{
    ast_free(cmd->ast_node);
    free(cmd);
}
void ast_print_shell_cmd(__attribute((unused)) struct ast_shell_cmd *cmd)
{
    errx(EXIT_FAILURE, "not implemented");
}
