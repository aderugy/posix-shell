#include "simple_command.h"

#include <err.h>
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
#include "utils/linked_list.h"
#include "utils/logger.h"

static char *keywords[] = { "then", "elif", "if",    "fi",    "else", "do",
                            "for",  "done", "while", "until", NULL };

bool is_keyword(char *word)
{
    for (size_t i = 0; keywords[i]; i++)
    {
        if (strcmp(keywords[i], word) == 0)
        {
            return true;
        }
    }
    return false;
}

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

    cmd->cmd = token->value.c;
    free(token->state);
    free(lexer_pop(lexer));

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

int simple_command_execute_builtin(struct ast_simple_cmd *cmd, char **argv,
                                   struct ast_eval_ctx *ctx)

{
    int element_count = cmd->args->size + 1;

    int *fd_ptr =
        calloc(3 * element_count, sizeof(int)); // alloc in case of redirections

    int *fd_pointer = fd_ptr;

    int ret_value = 0;

    int argc = 1;

    for (int i = 1; i < element_count; i++)
    {
        ctx->check_redir = true;
        struct ast_node *children = list_get(cmd->args, i - 1);
        if (ast_eval(children, (void **)&fd_pointer, ctx) == 1)
        {
            fd_pointer += 3; // for the 3 files descriptor that we need to close
        }
        else
        {
            argc++;
        }
    }

    logger("simple command : execute : %s\n", argv[0]);

    ret_value = run_command(argc, argv);

    fd_pointer = fd_ptr;
    while (*fd_pointer)
    {
        logger("simple_command ; fd to close : %i\n", *fd_pointer);
        close(*fd_pointer);

        dup2(fd_pointer[2], STDOUT_FILENO);

        close(fd_pointer[2]);

        fd_pointer += 3;
        logger("simple_command ; fd end : %i\n", *fd_pointer);
    }

    free(fd_ptr);
    return ret_value;
}
int simple_command_execute_non_builtin(struct ast_simple_cmd *cmd, char **argv,
                                       struct ast_eval_ctx *ctx, int argc)

{
    int elt = 1;

    int stat;
    int ret_value;

    logger("simple command : not a builtin\n");

    pid_t p = fork();

    if (p == 0)
    {
        for (int i = 1; i < argc; i++)
        {
            ctx->check_redir = true;
            struct ast_node *children = list_get(cmd->args, i - 1);

            if (ast_eval(children, NULL, ctx) == 0)
                elt++;
        }

        logger("simple_command.c : execute : %s\n", argv[0]);

        ret_value = execvp(argv[0], argv);
        exit(ret_value);
    }
    else
    {
        wait(&stat);

        int result = WEXITSTATUS(stat);
        if (result == 255)
        {
            errx(127, "simple_command: command not found");
        }
        return result;
    }
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
            // logger("  simple_connad.c : found arg : %s\n", argv[elt]);
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
