#include "shell_command.h"

#include <err.h>
#include <stdlib.h>

#include "node.h"
#include "utils/logger.h"

/*
 * shell_command = rule_if ;
 */
struct ast_shell_cmd *ast_parse_shell_cmd(struct lexer *lexer)
{
    logger("Parse SHELL_COMMAND\n");
    struct ast_shell_cmd *node = calloc(1, sizeof(struct ast_shell_cmd));
    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    // CASE 1 IF
    struct ast_node *rule = ast_create(lexer, AST_IF);
    if (rule)
    {
        node->ast_node = rule;
        return node;
    }
    logger("shell_command : if did not match\n");

    // CASE 2 WHILE
    rule = ast_create(lexer, AST_WHILE);
    if (rule)
    {
        node->ast_node = rule;
        return node;
    }
    logger("shell_command : while did not match\n");

    // CASE 3 UNTIL
    rule = ast_create(lexer, AST_UNTIL);
    if (rule)
    {
        node->ast_node = rule;
        return node;
    }
    logger("shell_command : until did not match\n");

    // CASE 4 FOR
    rule = ast_create(lexer, AST_FOR);
    if (!rule)
    {
        logger("shell_command : no rule matched\n");
        ast_free_shell_cmd(node);
        return NULL;
    }

    node->ast_node = rule;
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
void ast_print_shell_cmd(struct ast_shell_cmd *cmd)
{
    ast_print(cmd->ast_node);
}
