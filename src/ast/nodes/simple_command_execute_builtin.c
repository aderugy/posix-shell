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

    for (int i = 1; i < element_count; i++)
    {
        logger("builtin: log %i\n", i);
        struct linked_list *linked_list = list_init();
        ctx->check_redir = true;
        struct ast_node *children = list_get(cmd->args, i - 1);

        ast_eval(children, linked_list, ctx);

        if (linked_list->head)
        {
            struct linked_list_element *output_head = linked_list->head;
            while (output_head)
            {
                struct eval_output *output = output_head->data;
                *fd_pointer = output->value.fd;
                output_head = output_head->next;
                fd_pointer++;
            }
            // fd_pointer += 3; // for the 3 files descriptor that we need to
            // close
        }
        else
        {
            argc++;
        }

        list_free(linked_list, (void (*)(void *))eval_output_free);
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
}
