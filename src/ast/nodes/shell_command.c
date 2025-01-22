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

    if (token && token->type == TOKEN_SUBSHELL)
    {
        struct stream *stream = stream_from_str(token->value.c);
        struct lexer *lexer2 = lexer_create(stream);
        struct ast_node *clist = ast_create(lexer2, AST_CLIST);
        if (clist)
        {
            logger("clist\n");
            node->ast_node = clist;
            node->is_sub_shell = 1;
            lexer_free(lexer2);
            token_free(lexer_pop(lexer));
            return node;
        }
        lexer_free(lexer2);
        token_free(lexer_pop(lexer));
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
