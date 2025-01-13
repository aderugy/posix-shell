#include "command.h"

#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "utils/logger.h"

static char *prefixes_shell_command[] = { "if", "while", NULL };

bool is_prefix_shell_command(char *word)
{
    for (size_t i = 0; prefixes_shell_command[i]; i++)
    {
        if (strcmp(prefixes_shell_command[i], word) == 0)
        {
            return true;
        }
    }
    return false;
}

struct ast_cmd *ast_parse_cmd(struct lexer *lexer)
{
    logger("Parse COMMAND\n");
    struct ast_cmd *node = calloc(1, sizeof(struct ast_cmd));
    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    struct token *token_prefix = lexer_peek(lexer);

    if (token_prefix->value.c && is_prefix_shell_command(token_prefix->value.c))
    {
        struct ast_node *shell_cmd = ast_create(lexer, AST_SHELL_COMMAND);
        if (!shell_cmd)
        {
            ast_free_cmd(node);
            logger("Exit COMMAND\n");
            return NULL;
        }

        node->redirs = list_init();
        struct ast_node *redir;
        while ((redir = ast_create(lexer, AST_REDIRECTION)))
        {
            list_append(node->redirs, redir);
        }

        node->type = SHELL_CMD;
        node->cmd = shell_cmd;
        logger("Exit COMMAND\n");
        return node;
    }
    else
    {
        struct ast_node *simple_cmd = ast_create(lexer, AST_SIMPLE_COMMAND);
        if (simple_cmd)
        {
            logger("command : simple command found\n");
            node->type = SIMPLE_CMD;
            node->cmd = simple_cmd;
            logger("Exit COMMAND\n");
            return node;
        }
    }
    ast_free_cmd(node);
    return NULL;
}

void ast_free_cmd(struct ast_cmd *node)
{
    ast_free(node->cmd);

    if (node->type == SHELL_CMD && node->redirs)
    {
        list_free(node->redirs, (void (*)(void *))ast_free);
    }
    free(node);
}

int ast_eval_cmd(struct ast_cmd *node, void **out)
{
    return ast_eval(node->cmd, out);
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
