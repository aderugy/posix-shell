#include "simple_command_execute_non_builtin.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils/logger.h"
#include "utils/xalloc.h"

int simple_command_execute_non_builtin(struct ast_simple_cmd *cmd, char **argv,
                                       struct ast_eval_ctx *ctx, int argc)

{
    int stat;
    int ret_value;

    int element_count = cmd->args->size + 1;

    pid_t p = fork();

    if (p == 0)
    {
        /* LOOP TO RUN ALL REDIRECTIONS AND SAVE FILE DESCRIPTORS */
        for (int i = 1; i < element_count; i++)
        {
            ctx->check_redir = true;
            struct ast_node *children = list_get(cmd->args, i - 1);

            ast_eval(children, NULL, ctx);
        }

        for (int i = 0; i < argc; i++)
        {
            logger("execute non built : %s\n", argv[i]);
        }
        /* ENSURE THAT ARGV IS A NULL TERMINATED ARRAY*/
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
