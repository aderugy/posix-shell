#include "redirection_stdout.h"

#include "utils/logger.h"

static const char *DIGITS_OUT = "0123456789";
int redir_stdout_file_a(struct ast_redir *node,
                        __attribute((unused)) struct linked_list *out,
                        __attribute((unused)) struct ast_eval_ctx *ctx)
{
    int fd2 = 1;
    if (node->number != -1)
    {
        fd2 = node->number;
    }

    int saved_stdout = dup(fd2);

    if (fcntl(fd2, F_SETFD, FD_CLOEXEC) == -1)
    {
        errx(EXIT_FAILURE, "Invalid file descriptor for redirection");
    }

    char *file = node->file;
    int fd = open(file, O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (fd == -1)
    {
        errx(EXIT_FAILURE, "eval_redir: no such file: %s", node->file);
    }

    if (dup2(fd, fd2) == -1)
        errx(2, "redir_eval: dup: error");
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
}

int redir_stdout_fd(struct ast_redir *node,
                    __attribute((unused)) struct linked_list *out,
                    __attribute((unused)) struct ast_eval_ctx *ctx)
{
    char *val = node->file;
    for (size_t i = 0; val[i]; i++)
    {
        if (!strchr(DIGITS_OUT, val[i]))
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
        errx(2, "redir_eval: dup: error");
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
}
int redir_stdout_file_notrunc(struct ast_redir *node,
                              __attribute((unused)) struct linked_list *out,
                              __attribute((unused)) struct ast_eval_ctx *ctx)
{
    int fd2 = 1;
    if (node->number != -1)
    {
        fd2 = node->number;
    }

    int saved_stdout = dup(fd2);

    if (fcntl(fd2, F_SETFD, FD_CLOEXEC) == -1)
    {
        errx(EXIT_FAILURE, "Invalid file descriptor for redirection");
    }

    char *file = node->file;
    int fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1)
    {
        errx(EXIT_FAILURE, "eval_redir: no such file: %s", node->file);
    }

    if (dup2(fd, fd2) == -1)
        errx(2, "redir_eval: dup: error");
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
}
int redir_stdout_file(struct ast_redir *node, struct linked_list *out,
                      __attribute((unused)) struct ast_eval_ctx *ctx)
{
    logger("eval the redirection\n");
    int fd2 = 1;
    if (node->number != -1)
    {
        logger("found node->number %i\n", node->number);
        fd2 = node->number;
    }

    int saved_stdout = dup(fd2);
    if (fcntl(fd2, F_SETFD, FD_CLOEXEC) == -1)
    {
        errx(EXIT_FAILURE, "Invalid file descriptor for redirection");
    }
    char *file = node->file;
    int fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1)
    {
        fprintf(stderr, "eval_redir: no such file: %s", node->file);
    }
    if (dup2(fd, fd2) == -1)
        errx(2, "redir_eval: dup: error");
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
}
