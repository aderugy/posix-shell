#include "redirection.h"

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "element.h"
#include "ionumber.h"
#include "node.h"
#include "utils/logger.h"

/*
 * redirection = [IONUMBER] ( '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' )
 * WORD ;
 */

int redir_stdout_file(struct ast_redir *redir, __attribute((unused)) void **out,
                      __attribute((unused)) struct ast_eval_ctx *ctx);
int redir_stdout_file_a(struct ast_redir *redir,
                        __attribute((unused)) void **out,
                        __attribute((unused)) struct ast_eval_ctx *ctx);
int redir_stdout_file_notrunc(struct ast_redir *redir,
                              __attribute((unused)) void **out,
                              __attribute((unused)) struct ast_eval_ctx *ctx);
int redir_file_stdin(struct ast_redir *redir, __attribute((unused)) void **out,
                     __attribute((unused)) struct ast_eval_ctx *ctx);
int redir_stdin_fd(struct ast_redir *redir, __attribute((unused)) void **out,
                   __attribute((unused)) struct ast_eval_ctx *ctx);
int redir_fopen_rw(struct ast_redir *redir, __attribute((unused)) void **out,
                   __attribute((unused)) struct ast_eval_ctx *ctx);
int redir_stdin_fd(struct ast_redir *redir, __attribute((unused)) void **out,
                   __attribute((unused)) struct ast_eval_ctx *ctx);

static const struct redirection REDIR_LIST[] = {
    { TOKEN_REDIR_STDOUT_FILE, redir_stdout_file, ">" },
    { TOKEN_REDIR_STDOUT_FILE_A, redir_stdout_file_a, ">>" },
    { TOKEN_REDIR_STDOUT_FILE_NOTRUNC, redir_stdout_file_notrunc, ">|" },
    { TOKEN_REDIR_FILE_STDIN, redir_file_stdin, "<" },
    { TOKEN_REDIR_STDIN_FD, redir_stdin_fd, "<&" },
    { TOKEN_REDIR_FOPEN_RW, redir_fopen_rw, "<>" },
    { TOKEN_REDIR_STDOUT_FD, redir_stdin_fd, ">&" }
};

#define REDIR_LEN (sizeof(REDIR_LIST) / sizeof(REDIR_LIST[0]))

static int is_redir(struct token *token)
{
    for (size_t i = 0; i < REDIR_LEN; i++)
    {
        if (token->type == REDIR_LIST[i].type)
        {
            return 1;
        }
    }

    return 0;
}

struct ast_redir *ast_parse_redir(struct lexer *lexer)
{
    logger("Parse REDIRECTION\n");
    struct ast_redir *redir = calloc(1, sizeof(struct ast_redir));
    if (!redir)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    char *file = NULL;
    // struct ast_node *number = ast_create(lexer, AST_IONUMBER);
    struct token *token = lexer_peek(lexer);
    if (!token || !is_redir(token))
    {
        goto error;
    }
    redir->pipe = token->type;
    if (!token->value.c)
    {
        logger("token->value NULL\n");
    }
    else
    {
        logger("token->value not NULL\n");
        logger("len: %i\n", strlen(token->value.c));
    }
    if (strlen(token->value.c) == 0)
    {
        redir->number = -1;
    }
    else
    {
        redir->number = atoi(token->value.c);
    }
    logger("redir->number = %i\n", redir->number);

    if (token->value.c)
    {
        free(token->value.c);
    }
    free(lexer_pop(lexer));

    token = lexer_peek(lexer);
    if (!token || token->type != TOKEN_WORD)
    {
        token = NULL;
        goto error;
    }

    redir->file = token->value.c;
    free(token->state);
    free(lexer_pop(lexer));

    return redir;

error:
    if (redir)
    {
        ast_free_redir(redir);
    }
    if (file)
    {
        free(file);
    }
    logger("Exit REDIRECTION\n");
    return NULL;
}

int redir_file_stdin(struct ast_redir *node, __attribute((unused)) void **out,
                     __attribute((unused)) struct ast_eval_ctx *ctx)
{
    int saved_stdout = dup(STDOUT_FILENO);
    int fd2 = 0;
    if (node->number != -1)
    {
        fd2 = node->number;
    }
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

int redir_stdout_file(struct ast_redir *node, void **out,
                      __attribute((unused)) struct ast_eval_ctx *ctx)
{
    int saved_stdout = dup(STDOUT_FILENO);
    logger("Eval redir_stdout_file\n");
    int fd2 = 1;
    if (node->number != -1)
    {
        fd2 = node->number;
    }
    logger("fd2: %i\n", fd2);
    if (fcntl(fd2, F_SETFD, FD_CLOEXEC) == -1)
    {
        errx(EXIT_FAILURE,
             "stdout_file: Invalid file descriptor for redirection");
    }
    char *file = node->file;
    int fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1)
    {
        errx(EXIT_FAILURE, "eval_redir: no such file: %s", node->file);
    }
    if (dup2(fd, fd2) == -1)
        errx(2, "redir_eval: dup: error");
    logger("Exit Eval Redir\n");
    if (out)
    {
        int *origin_fd = *out;
        *origin_fd = fd;
        *(origin_fd + 1) = fd2;
        *(origin_fd + 2) = saved_stdout;
    }
    return 0;
}

int redir_stdout_file_a(struct ast_redir *node,
                        __attribute((unused)) void **out,
                        __attribute((unused)) struct ast_eval_ctx *ctx)
{
    int saved_stdout = dup(STDOUT_FILENO);
    logger("Eval redir_stdout_file_a\n");
    int fd2 = 1;
    if (node->number != -1)
    {
        fd2 = node->number;
    }

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
        int *origin_fd = *out;
        *origin_fd = fd;
        *(origin_fd + 1) = fd2;
        *(origin_fd + 2) = saved_stdout;
    }
    logger("Exit Eval Redir\n");
    return 0;
}

int redir_stdout_fd(struct ast_redir *node, __attribute((unused)) void **out,
                    __attribute((unused)) struct ast_eval_ctx *ctx)
{
    int saved_stdout = dup(STDOUT_FILENO);
    int fd2 = 1;
    if (node->number != -1)
    {
        fd2 = node->number;
    }

    if (fcntl(fd2, F_SETFD, FD_CLOEXEC) == -1)
    {
        errx(EXIT_FAILURE, "Invalid file descriptor for redirection");
    }
    int fd = atoi(node->file);

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

int redir_stdin_fd(struct ast_redir *node, __attribute((unused)) void **out,
                   __attribute((unused)) struct ast_eval_ctx *ctx)
{
    int saved_stdout = dup(STDOUT_FILENO);
    int fd2 = 1;
    if (node->number != -1)
    {
        fd2 = node->number;
    }

    if (fcntl(fd2, F_SETFD, FD_CLOEXEC) == -1)
    {
        errx(EXIT_FAILURE, "Invalid file descriptor for redirection");
    }
    int fd = atoi(node->file);

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

int redir_fopen_rw(struct ast_redir *node, __attribute((unused)) void **out,
                   __attribute((unused)) struct ast_eval_ctx *ctx)
{
    int saved_stdout = dup(STDOUT_FILENO);
    int fd2 = 0;
    if (node->number != -1)
    {
        fd2 = node->number;
    }
    char *file = node->file;

    if (fcntl(fd2, F_SETFD, FD_CLOEXEC) == -1)
    {
        errx(EXIT_FAILURE, "Invalid file descriptor for redirection");
    }
    int fd = open(file, O_RDWR);
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

int redir_stdout_file_notrunc(struct ast_redir *node,
                              __attribute((unused)) void **out,
                              __attribute((unused)) struct ast_eval_ctx *ctx)
{
    int saved_stdout = dup(STDOUT_FILENO);
    int fd2 = 1;
    if (node->number != -1)
    {
        fd2 = node->number;
    }

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
    logger("Exit Eval Redir\n");
    if (out)
    {
        int *origin_fd = *out;
        *origin_fd = fd;
        *(origin_fd + 1) = fd2;
        *(origin_fd + 2) = saved_stdout;
    }
    return 0;
}

int ast_eval_redir(struct ast_redir *node, void **out,
                   __attribute((unused)) struct ast_eval_ctx *ctx)
{
    for (size_t i = 0; i < REDIR_LEN; i++)
    {
        if (node->pipe == REDIR_LIST[i].type)
        {
            return REDIR_LIST[i].redir(node, out, NULL);
        }
    }
    errx(2, "eval_redir: error");
}

void ast_free_redir(struct ast_redir *node)
{
    free(node->file);
    free(node);
}

void ast_print_redir(struct ast_redir *node)
{
    logger("redir ");
    if (node->number)
    {
        logger("Number: %lu", node->number);
    }
    if (node->pipe)
    {
        for (size_t i = 0; i < REDIR_LEN; i++)
        {
            if (REDIR_LIST[i].type == node->pipe)
            {
                logger("%s", REDIR_LIST[i].token);
                break;
            }
        }
    }
    if (node->file)
    {
        logger("%s", node->file);
    }
}
