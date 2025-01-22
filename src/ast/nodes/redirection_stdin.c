#include "redirection_stdin.h"

#include <ctype.h>
#include <stdio.h>

#include "node.h"

int redir_file_stdin(struct ast_redir *node,
                     __attribute((unused)) struct linked_list *out,
                     __attribute((unused)) struct ast_eval_ctx *ctx)
{
    int fd2 = 0;
    if (node->number != -1)
    {
        fd2 = node->number;
    }

    int saved_stdout = dup(fd2);
    struct linked_list *filenames = list_init();
    if (ast_eval(node->file, filenames, ctx) == AST_EVAL_ERROR
        || filenames->size != 1)
    {
        goto error;
    }

    struct eval_output *filename = filenames->head->data;
    if (filename->type != EVAL_STR)
    {
        goto error;
    }

    char *file = filename->value.str;
    if (fcntl(fd2, F_SETFD, FD_CLOEXEC) == -1)
    {
        errx(EXIT_FAILURE, "Invalid file descriptor for redirection");
    }

    int fd = open(file, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "eval_redir: no such file: %s\n", file);
        return 1;
    }

    if (dup2(fd, fd2) == -1)
    {
        errx(2, "redir_eval: dup: error");
    }
    if (out)
    {
        struct eval_output *eval_output_fd_1 = eval_output_init();
        struct eval_output *eval_output_fd_2 = eval_output_init();
        struct eval_output *eval_output_fd_3 = eval_output_init();

        eval_output_fd_1->value.fd = fd;
        eval_output_fd_2->value.fd = fd2;
        eval_output_fd_3->value.fd = saved_stdout;

        list_append(out, eval_output_fd_1);
        list_append(out, eval_output_fd_2);
        list_append(out, eval_output_fd_3);
    }
    return 0;
error:
    // @TODO
    return AST_EVAL_ERROR;
}
int redir_stdin_fd(struct ast_redir *node,
                   __attribute((unused)) struct linked_list *out,
                   __attribute((unused)) struct ast_eval_ctx *ctx)
{
    struct linked_list *filenames = list_init();
    if (ast_eval(node->file, filenames, ctx) == AST_EVAL_ERROR
        || filenames->size != 1)
    {
        goto error;
    }

    struct eval_output *filename = filenames->head->data;
    if (filename->type != EVAL_STR)
    {
        goto error;
    }

    char *val = filename->value.str;
    for (size_t i = 0; val[i]; i++)
    {
        if (!isdigit(val[i]))
        {
            errx(2, "redir_stdout_fd: not a number");
        }
    }

    int fd2 = 1;
    if (node->number != -1)
    {
        fd2 = node->number;
    }

    int saved_stdout = dup(fd2);
    int fd = atoi(val);
    if (fcntl(fd2, F_SETFD, FD_CLOEXEC) == -1)
    {
        errx(EXIT_FAILURE, "Invalid file descriptor for redirection");
    }

    if (fcntl(fd, F_SETFD) == -1)
    {
        errx(EXIT_FAILURE, "Invalid file descriptor for redirection");
    }

    if (dup2(fd, fd2) == -1)
    {
        errx(2, "redir_eval: dup: error");
    }
    if (out)
    {
        struct eval_output *eval_output_fd_1 = eval_output_init();
        struct eval_output *eval_output_fd_2 = eval_output_init();
        struct eval_output *eval_output_fd_3 = eval_output_init();

        eval_output_fd_1->value.fd = fd;
        eval_output_fd_2->value.fd = fd2;
        eval_output_fd_3->value.fd = saved_stdout;

        list_append(out, eval_output_fd_1);
        list_append(out, eval_output_fd_2);
        list_append(out, eval_output_fd_3);
    }

    return 0;

error:
    return AST_EVAL_ERROR;
}
