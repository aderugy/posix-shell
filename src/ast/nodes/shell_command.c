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
    struct ast_shell_cmd *node = calloc(1, sizeof(struct ast_shell_cmd));
    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    logger("Parse SHELL_COMMAND\n");
    // CASE 1 IF
    struct ast_node *rule = ast_create(lexer, AST_IF);
    if (rule)
    {
        node->ast_node = rule;
        return node;
    }

    // CASE 2 WHILE
    rule = ast_create(lexer, AST_WHILE);
    if (rule)
    {
        node->ast_node = rule;
        return node;
    }

    // CASE 3 UNTIL
    rule = ast_create(lexer, AST_UNTIL);
    if (rule)
    {
        node->ast_node = rule;
        return node;
    }

    // CASE 4 FOR
    rule = ast_create(lexer, AST_FOR);
    if (!rule)
    {
        ast_free_shell_cmd(node);
        logger("Exit SHELL COMMAND (FAILED)\n");
        return NULL;
    }

    node->ast_node = rule;
    return node;
}
int ast_eval_shell_cmd(struct ast_shell_cmd *cmd, void **ptr,
                       struct ast_eval_ctx *ctx)
{
    return ast_eval(cmd->ast_node, ptr, ctx);
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
