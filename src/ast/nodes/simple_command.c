#include "simple_command.h"

#include <err.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast/ast.h"
#include "builtins/builtins.h"
#include "builtins/commands.h"
#include "builtins/run_command.h"
#include "lexer/token.h"
#include "node.h"
#include "simple_command_execute_builtin.h"
#include "simple_command_execute_non_builtin.h"
#include "utils/linked_list.h"
#include "utils/logger.h"
#include "utils/naming.h"
#include "utils/xalloc.h"

struct ast_simple_cmd *ast_parse_simple_cmd(struct lexer *lexer)
{
    struct ast_simple_cmd *cmd = xcalloc(1, sizeof(struct ast_simple_cmd));

    struct token *token = NULL;
    cmd->prefix = ast_create(lexer, AST_PREFIX);
    cmd->prefixes = list_init();
    cmd->cmd = NULL;
    cmd->args = list_init();

    struct ast_node *prefix;
    while ((prefix = ast_create(lexer, AST_PREFIX)))
    {
        list_append(cmd->prefixes, prefix);
    }

    logger("Parse SIMPLE_COMMAND\n");
    if (cmd->prefix)
    {
        // prefix { prefix }
        logger("Exit SIMPLE_COMMAND (SUCCESS)\n");
        return cmd;
    }

    // { prefix } WORD { element }
    token = lexer_peek(lexer);
    if (!token || token->type != TOKEN_WORD
        || (token->value.c && is_keyword(token->value.c)))
    {
        goto error;
    }

    struct token *parenthese = lexer_peek_two(lexer);
    if (parenthese && parenthese->type == TOKEN_WORD
        && strcmp(parenthese->value.c, "(") == 0)
    {
        goto error;
    }

    cmd->cmd = strdup(token->value.c);
    token_free(lexer_pop(lexer));

    struct ast_node *element;
    while ((element = ast_create(lexer, AST_ELEMENT)))
    {
        list_append(cmd->args, element);
    }

    logger("Exit SIMPLE_COMMAND (SUCCESS)\n");
    return cmd;
error:
    ast_free_simple_cmd(cmd);
    logger("Exit SIMPLE_COMMAND (ERROR)\n");
    return NULL;
}

int ast_eval_simple_cmd(struct ast_simple_cmd *cmd,
                        __attribute((unused)) void **out,
                        struct ast_eval_ctx *ctx)
{
    if (!cmd->cmd)
    {
        logger("Eval SIMPLE_COMMAND: RULE 1\n");
        return ast_eval(cmd->prefix, NULL, ctx);
    }
    logger("Eval SIMPLE_COMMAND: RULE 2\n");

    int argc = cmd->args->size + 1;
    char **argv = calloc(argc + 1, sizeof(char *));

    argv[0] = cmd->cmd;

    size_t elt = 1;

    for (int i = 1; i < argc; i++)
    {
        ctx->check_redir = false;
        struct ast_node *children = list_get(cmd->args, i - 1);
        if (ast_eval(children, (void **)argv + elt, ctx) == 0)
        {
            elt++;
        }
    }

    struct runnable *cmd_runnable =
        get_command(argv[0], NULL); // get the builtin if exists

    int ret_value = 0;
    if (cmd_runnable) // check if it is a builtin
    {
        ret_value = simple_command_execute_builtin(cmd, argv, ctx);
    }
    else
    {
        ret_value = simple_command_execute_non_builtin(cmd, argv, ctx, argc);
    }

    for (size_t i = 1; i <= elt; i++)
    {
        free(argv[i]);
    }
    free(argv);
    return ret_value;
}

void ast_free_simple_cmd(struct ast_simple_cmd *cmd)
{
    if (!cmd)
    {
        return;
    }

    if (cmd->prefix)
    {
        ast_free(cmd->prefix);
    }
    list_free(cmd->prefixes, (void (*)(void *))ast_free);

    if (cmd->cmd)
    {
        free(cmd->cmd);
    }
    list_free(cmd->args, (void (*)(void *))ast_free);

    free(cmd);
}

void ast_print_simple_cmd(struct ast_simple_cmd *cmd)
{
    logger("command");

    struct linked_list_element *head = cmd->args->head;
    while (head)
    {
        struct ast_node *node = head->data;
        logger(" ");
        ast_print(node);
        head = head->next;
    }
}
