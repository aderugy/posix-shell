#include "shell_command.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "node.h"
#include "utils/logger.h"
#include "utils/xalloc.h"

/*
 * shell_command =
                '(' compound_list ')'
                | '{' compound_list '}'
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

    struct token *token = lexer_peek(lexer);
    if (!token)
    {
        goto error;
    }

    if (token->type == TOKEN_LEFT_BRACKET)
    {
        token_free(lexer_pop(lexer));

        struct ast_node *clist = ast_create(lexer, AST_CLIST);
        if (!clist)
        {
            lexer_error(lexer, "expected body");
            goto error;
        }

        token = lexer_peek(lexer);
        if (!token || token->type != TOKEN_RIGHT_BRACKET)
        {
            lexer_error(lexer, "unmatched bracket");
            goto error;
        }

        token_free(lexer_pop(lexer));
        node->ast_node = clist;
        return node;
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
        goto error;
    }

    node->ast_node = rule;
    return node;

error:
    logger("Exit SHELL COMMAND (FAILED)\n");
    ast_free_shell_cmd(node);
    return NULL;
}
int ast_eval_shell_cmd(struct ast_shell_cmd *cmd, struct linked_list *out,
                       struct ast_eval_ctx *ctx)
{
    if (cmd->is_sub_shell == 1)
    {
        pid_t p;
        p = fork();
        int status;
        if (p < 0)
        {
            exit(ast_eval(cmd->ast_node, NULL, ctx));
        }
        // child process because return value zero
        else if (p == 0)
        {
            exit(ast_eval(cmd->ast_node, NULL, ctx));
        }
        waitpid(p, &status, 0);
        return WEXITSTATUS(status);
    }
    return ast_eval(cmd->ast_node, out, ctx);
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
