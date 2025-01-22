#include "simple_command_execute_non_builtin.h"
#include "utils/xalloc.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils/logger.h"

int simple_command_execute_non_builtin(struct ast_simple_cmd *cmd, char **argv,
                                       struct ast_eval_ctx *ctx, int argc)

{
    int stat;
    int ret_value;

    int element_count = cmd->args->size + 1;

    pid_t p = fork();

    if (p == 0)
    {
        for (int i = 1; i < element_count; i++)
        {
            ctx->check_redir = true;
            struct ast_node *children = list_get(cmd->args, i - 1);

            ast_eval(children, NULL, ctx);
        }

        logger("simple_command.c : execute : %s\n", argv[0]);
        for (int i = 1; i < argc; i++)
        {
            logger("execute non built : %s\n", argv[i]);
        }

        argv = xrealloc(argv, (argc + 1) * sizeof(char *));
        argv[argc] = NULL;

        ret_value = execvp(argv[0], argv);
        exit(ret_value);
    }
    else
    {
        wait(&stat);

        int result = WEXITSTATUS(stat);
        if (result == 255)
        {
            warnx("simple_command: command not found");
            result = 127;
        }
        return result;
    }
}
