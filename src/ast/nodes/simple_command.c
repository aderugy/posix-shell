#include "simple_command.h"

#include <err.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast/ast.h"
#include "builtins/builtins.h"
#include "builtins/commands.h"
#include "builtins/run_command.h"
#include "lexer/token.h"
#include "node.h"
#include "utils/linked_list.h"
#include "utils/logger.h"

struct ast_simple_cmd *ast_parse_simple_cmd(struct lexer *lexer)
{
    struct ast_simple_cmd *cmd = calloc(1, sizeof(struct ast_simple_cmd));
    if (!cmd)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

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

    if (cmd->prefix)
    {
        // prefix { prefix }
        return cmd;
    }

    // { prefix } WORD { element }
    token = lexer_peek(lexer);
    if (token->type != TOKEN_WORD)
    {
        goto error;
    }

    cmd->cmd = token->value.c;
    free(lexer_pop(lexer));

    struct ast_node *element;
    while ((element = ast_create(lexer, AST_ELEMENT)))
    {
        list_append(cmd->args, element);
    }

    return cmd;
error:
    ast_free_simple_cmd(cmd);
    return NULL;
}

int ast_eval_simple_cmd(struct ast_simple_cmd *cmd,
                        __attribute((unused)) void **out)
{
    size_t argc = cmd->args->size;
    char **argv = calloc(argc, sizeof(char *));

    for (size_t i = 0; i < argc; i++)
    {
        struct ast_node *children = list_get(cmd->args, i);
        ast_eval(children, (void **)argv + i);
    }
    logger("simple commad : execute : %s\n", argv[0]);
    int ret_value = run_command(argc, argv);
    int stat;
    if (ret_value == 127)
    {
        logger("simple command : not a builtin\n");

        pid_t p = fork();
        if (p == 0)
        {
            ret_value = execvp(argv[0], argv);
            exit(ret_value);
        }
        else
        {
            wait(&stat);

            free(argv);
            return WEXITSTATUS(stat);
        }
    }

    free(argv);
    return ret_value;
}

void ast_free_simple_cmd(struct ast_simple_cmd *cmd)
{
    if (cmd->prefix)
    {
        ast_free(cmd->prefix);
    }
    list_free(cmd->prefixes, (void (*)(void *))ast_free);

    if (cmd)
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
