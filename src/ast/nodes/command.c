#include "command.h"

#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "simple_command.h"
#include "utils/logger.h"
#include "utils/xalloc.h"

/*
    command =
            | simple_command
            | simple_command { redirection }
            | fundec { redirection }
            ;
*/
struct ast_cmd *ast_parse_cmd(struct lexer *lexer)
{
    logger("Parse COMMAND\n");

    struct ast_cmd *node = xcalloc(1, sizeof(struct ast_cmd));

    struct ast_node *simple_cmd = ast_create(lexer, AST_SIMPLE_COMMAND);
    if (simple_cmd)
    {
        node->type = SIMPLE_CMD;
        node->cmd = simple_cmd;
        logger("Exit COMMAND (SUCCESS)\n");
        return node;
    }

    struct ast_node *shell_cmd = ast_create(lexer, AST_SHELL_COMMAND);
    if (shell_cmd)
    {
        node->redirs = list_init();
        struct ast_node *redir;
        while ((redir = ast_create(lexer, AST_REDIRECTION)))
        {
            list_append(node->redirs, redir);
        }

        node->type = SHELL_CMD;
        node->cmd = shell_cmd;
        logger("Exit COMMAND (SUCCESS)\n");
        return node;
    }

    // @Remark : redundant code, could be merged
    struct ast_node *fundec = ast_create(lexer, AST_FUNDEC);
    if (fundec)
    {
        node->redirs = list_init();
        struct ast_node *redir;
        while ((redir = ast_create(lexer, AST_REDIRECTION)))
        {
            list_append(node->redirs, redir);
        }

        node->type = FUNDEC;
        node->cmd = fundec;
        logger("Exit COMMAND (SUCCESS)\n");
        return node;
    }

    logger("Exit COMMAND (ERROR)\n");
    ast_free_cmd(node);
    return NULL;
}

void ast_free_cmd(struct ast_cmd *node)
{
    ast_free(node->cmd);

    if (node->type == SHELL_CMD || node->type == FUNDEC)
    {
        if (node->redirs)
        {
            list_free(node->redirs, (void (*)(void *))ast_free);
        }
    }
    free(node);
}

int ast_eval_cmd(struct ast_cmd *node, struct linked_list *out,
                 struct ast_eval_ctx *ctx)
{
    return ast_eval(node->cmd, out, ctx);
}

void ast_print_cmd(struct ast_cmd *node)
{
    ast_print(node->cmd);

    if (node->type == SHELL_CMD && node->redirs)
    {
        struct linked_list *list = node->redirs;
        struct linked_list_element *head = list->head;
        while (head)
        {
            logger(" ");
            ast_print(head->data);
            head = head->next;
        }
    }
}
