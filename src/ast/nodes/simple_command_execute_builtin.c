#include "simple_command_execute_builtin.h"

#include <err.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins/run_command.h"
#include "utils/logger.h"
#include "utils/xalloc.h"

int simple_command_execute_builtin(struct ast_simple_cmd *cmd, char **argv,
                                   struct ast_eval_ctx *ctx)

{
    logger("execute bultin\n");
    int element_count = cmd->args->size + 1;

    int *fd_ptr = xcalloc(3 * element_count,
                          sizeof(int)); // alloc in case of redirections

    int *fd_pointer = fd_ptr;

    int ret_value = 0;

    int argc = 1;

    struct linked_list *linked_list = list_init();
    for (int i = 1; i < element_count; i++)
    {
        ctx->check_redir = true;
        struct ast_node *children = list_get(cmd->args, i - 1);

        int element_eval_result;
        if ((element_eval_result =
                 ast_eval(children, /*(void **)&fd_pointer*/ linked_list, ctx))
            == -1)
        {
            fd_pointer += 3; // for the 3 files descriptor that we need to close
        }
        else if (element_eval_result == 0)
        {
            argc++;
        }
        else
        {
            ret_value = element_eval_result;
            goto error;
        }
    }

    logger("simple command : execute : %s\n", argv[0]);
    for (int i = 0; i < argc; i++)
    {
        logger("builtin execute : %s\n", argv[i]);
    }

    ret_value = run_command(argc, argv, ctx);

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
error:

    if (fd_ptr)
    {
        free(fd_ptr);
    }

    logger("Exit SIMPLE_COMMAND with error\n");
    return ret_value;
}
