#include "simple_command.h"

#include <err.h>
#include <stddef.h>
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
#include "utils/linked_list.h"
#include "utils/logger.h"

struct ast_simple_cmd *ast_parse_simple_cmd(struct lexer *lexer)
{
    struct token *tok_cmd = lexer_peek(lexer);
    if (tok_cmd && tok_cmd->value.c && strcmp("if", tok_cmd->value.c) == 0)
    {
        return NULL;
    }

    struct ast_node *cmd = ast_create(lexer, AST_ELEMENT);

    struct ast_simple_cmd *simple_cmd =
        calloc(1, sizeof(struct ast_simple_cmd));
    if (!simple_cmd)
    {
        errx(AST_PARSE_ERROR, "out of memory");
    }

    struct linked_list *list = list_init();
    do
    {
        list_append(list, cmd);
    } while ((cmd = ast_create(lexer, AST_ELEMENT)));

    simple_cmd->args = list;
    return simple_cmd;
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
