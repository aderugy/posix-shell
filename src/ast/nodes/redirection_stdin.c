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
        struct eval_output *eval_output_fd_1 = eval_output_init(EVAL_FD);
        struct eval_output *eval_output_fd_2 = eval_output_init(EVAL_FD);
        struct eval_output *eval_output_fd_3 = eval_output_init(EVAL_FD);

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
    int fd = -1;
    int fd2 = -1;
    int saved_stdout = -1;

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

    fd2 = 1;
    if (node->number != -1)
    {
        fd2 = node->number;
    }

    saved_stdout = dup(fd2);
    fd = atoi(val);
    if (fcntl(fd2, F_SETFD, FD_CLOEXEC) == -1)
    {
        goto error;
    }

    if (fcntl(fd, F_SETFD) == -1)
    {
        goto error;
    }

    if (dup2(fd, fd2) == -1)
    {
        goto error;
    }
    if (out)
    {
        struct eval_output *eval_output_fd_1 = eval_output_init(EVAL_FD);
        struct eval_output *eval_output_fd_2 = eval_output_init(EVAL_FD);
        struct eval_output *eval_output_fd_3 = eval_output_init(EVAL_FD);

        eval_output_fd_1->value.fd = fd;
        eval_output_fd_2->value.fd = fd2;
        eval_output_fd_3->value.fd = saved_stdout;

        list_append(out, eval_output_fd_1);
        list_append(out, eval_output_fd_2);
        list_append(out, eval_output_fd_3);
    }

    return 0;

error:
    if (fd != -1)
    {
        close(fd);
    }
    if (fd2 != -1)
    {
        close(fd2);
    }
    if (saved_stdout != -1)
    {
        close(saved_stdout);
    }

    list_free(filenames, (void (*)(void *))eval_output_free);
    return AST_EVAL_ERROR;
}
