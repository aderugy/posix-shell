#include "redirection_stdin.h"

static const char *DIGITS = "0123456789";
int redir_file_stdin(struct ast_redir *node, __attribute((unused)) void **out,
                     __attribute((unused)) struct ast_eval_ctx *ctx)
{
    int fd2 = 0;
    if (node->number != -1)
    {
        fd2 = node->number;
    }

    int saved_stdout = dup(fd2);
    char *file = node->file;

    if (fcntl(fd2, F_SETFD, FD_CLOEXEC) == -1)
    {
        errx(EXIT_FAILURE, "Invalid file descriptor for redirection");
    }
    int fd = open(file, O_RDONLY);
    if (fd == -1)
    {
        errx(EXIT_FAILURE, "eval_redir: no such file: %s", node->file);
    }
    if (dup2(fd, fd2) == -1)
        errx(2, "redir_eval: dup: error");
    if (out)
    {
        int *origin_fd = *out;
        *origin_fd = fd;
        *(origin_fd + 1) = fd2;
        *(origin_fd + 2) = saved_stdout;
    }
    return 0;
}
int redir_stdin_fd(struct ast_redir *node, __attribute((unused)) void **out,
                   __attribute((unused)) struct ast_eval_ctx *ctx)
{
    char *val = node->file;
    for (size_t i = 0; val[i]; i++)
    {
        if (!strchr(DIGITS, val[i]))
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
        int *origin_fd = *out;
        *origin_fd = fd;
        *(origin_fd + 1) = fd2;
        *(origin_fd + 2) = saved_stdout;
    }
    return 0;
}
