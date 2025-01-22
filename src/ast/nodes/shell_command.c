#include "shell_command.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "utils/logger.h"
#include "utils/xalloc.h"

/*
 * shell_command =
                '{' compound_list '}'
                | rule_if
                | rule_while
                | rule_until
                | rule_for
                ;
 */
struct ast_shell_cmd *ast_parse_shell_cmd(struct lexer *lexer)
{
    logger("Parse SHELL_COMMAND\n");
    struct ast_shell_cmd *node = xcalloc(1, sizeof(struct ast_shell_cmd));

    // CASE 0 '{' compound_list '}'
    struct token *token = lexer_peek(lexer);
    token_print(token);
    if (TOKEN_OK && strcmp(token->value.c, "{") == 0)
    {
        token_free(lexer_pop(lexer));

        struct ast_node *clist = ast_create(lexer, AST_CLIST);
        if (clist)
        {
            token = lexer_pop(lexer);
            if (TOKEN_OK && strcmp(token->value.c, "}") == 0)
            {
                token_free(token);
                node->ast_node = clist;
                return node;
            }
            token_free(token);
        }

        lexer_error(lexer, "unmatched bracket");
        ast_free_shell_cmd(node);
        return NULL;
    }

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
