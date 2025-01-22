#include "simple_command_execute_non_builtin.h"

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
    int elt = 1;

    int stat;
    int ret_value;

    logger("simple command : not a builtin\n");
    for (int i = 0; i < argc; i++)
    {
        logger("exec : %s\n", argv[i]);
    }

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
            warnx("simple_command: command not found");
            result = 127;
        }
        return result;
    }
}
