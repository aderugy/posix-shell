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
static const char *DIGITS = "0123456789";
static const enum token_type REDIRECTIONS[] = { TOKEN_REDIR_STDOUT_FILE,
                                                TOKEN_REDIR_STDOUT_FILE_A,
                                                TOKEN_REDIR_STDOUT_FILE_NOTRUNC,
                                                TOKEN_REDIR_FILE_STDIN,
                                                TOKEN_REDIR_STDIN_FD,
                                                TOKEN_REDIR_FOPEN_RW,
                                                0 };

static int is_redir(struct token *token)
{
    for (int i = 0; REDIRECTIONS[i]; i++)
    {
        if (token->type == REDIRECTIONS[i])
        {
            return 1;
        }
    }

    return 0;
}

struct ast_redir *ast_parse_redir(struct lexer *lexer)
{
    logger("\t\t\tParse REDIRECTION\n");
    struct ast_redir *redir = calloc(1, sizeof(struct ast_redir));
    if (!redir)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    char *file = NULL;
    struct ast_node *number = ast_create(lexer, AST_IONUMBER);
    struct token *token = lexer_peek(lexer);

    if (!token || !is_redir(token))
    {
        goto error;
    }
    redir->pipe = token->type;

    free(lexer_pop(lexer));
    token = lexer_peek(lexer);
    if (!token || token->type != TOKEN_WORD)
    {
        token = NULL;
        goto error;
    }

    redir->number = number;
    redir->file = token->value.c;
    free(token->state);
    free(lexer_pop(lexer));

    return redir;

error:
    if (number)
    {
        ast_free(number);
    }
    if (redir)
    {
        ast_free_redir(redir);
    }
    if (file)
    {
        free(file);
    }
    logger("\t\t\tExit REDIRECTION\n");
    return NULL;
}

int redir_file_stdin(struct ast_redir *node, __attribute((unused)) void **out)
{
    int fd2 = 0;
    if (node->number)
    {
        fd2 = ast_eval(node->number, NULL);
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
    return 0;
}

int redir_stdout_file(struct ast_redir *node, void **out)
{
    int saved_stdout = dup(STDOUT_FILENO);
    logger("Eval redir_stdout_file\n");
    int fd2 = 1;
    if (node->number)
    {
        fd2 = ast_eval(node->number, NULL);
    }
    if (fcntl(fd2, F_SETFD, FD_CLOEXEC) == -1)
    {
        errx(EXIT_FAILURE, "Invalid file descriptor for redirection");
    }
    char *file = node->file;
    int fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    // logger("\t\tfd : %i\n", fd);
    if (fd == -1)
    {
        errx(EXIT_FAILURE, "eval_redir: no such file: %s", node->file);
    }
    if (dup2(fd, fd2) == -1)
        errx(2, "redir_eval: dup: error");
    logger("\t\tExit Eval Redir\n");
    if (out)
    {
        // logger("\t\tfd2 : %i\n", fd2);
        int *origin_fd = *out;
        *origin_fd = fd;
        *(origin_fd + 1) = fd2;
        *(origin_fd + 2) = saved_stdout;
    }
    return 0;
}

int redir_stdout_file_a(struct ast_redir *node,
                        __attribute((unused)) void **out)
{
    logger("Eval redir_stdout_file_a\n");
    int fd2 = 1;
    if (node->number)
    {
        fd2 = ast_eval(node->number, NULL);
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
    logger("Exit Eval Redir\n");
    return 0;
}

int redir_stdout_fd(struct ast_redir *node, __attribute((unused)) void **out)
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
    if (node->number)
    {
        fd2 = ast_eval(node->number, NULL);
    }

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
    return 0;
}

int redir_stdin_fd(struct ast_redir *node, __attribute((unused)) void **out)
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
    if (node->number)
    {
        fd2 = ast_eval(node->number, NULL);
    }

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
    return 0;
}

int redir_fopen_rw(struct ast_redir *node, __attribute((unused)) void **out)
{
    int fd2 = 0;
    if (node->number)
    {
        fd2 = ast_eval(node->number, NULL);
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
    return 0;
}

int redir_stdout_file_notrunc(struct ast_redir *node,
                              __attribute((unused)) void **out)
{
    int fd2 = 1;
    if (node->number)
    {
        fd2 = ast_eval(node->number, NULL);
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
    return 0;
}

static const struct redirection REDIR_LIST[] = {
    { TOKEN_REDIR_STDOUT_FILE, redir_stdout_file },
    { TOKEN_REDIR_STDOUT_FILE_A, redir_stdout_file_a },
    { TOKEN_REDIR_STDOUT_FILE_NOTRUNC, redir_stdout_file_notrunc },
    { TOKEN_REDIR_FILE_STDIN, redir_file_stdin },
    { TOKEN_REDIR_STDIN_FD, redir_stdin_fd },
    { TOKEN_REDIR_FOPEN_RW, redir_fopen_rw },
    { TOKEN_REDIR_STDOUT_FD, redir_stdin_fd }
};

#define REDIR_LEN (sizeof(REDIR_LIST) / sizeof(REDIR_LIST[0]))

int ast_eval_redir(struct ast_redir *node, void **out)
{
    for (size_t i = 0; i < REDIR_LEN; i++)
    {
        if (node->pipe == REDIR_LIST[i].type)
        {
            return REDIR_LIST[i].redir(node, out);
        }
    }
    errx(2, "eval_redir: error");
}

void ast_free_redir(struct ast_redir *node)
{
    free(node->file);
    ast_free(node->number);
    free(node);
}

void ast_print_redir(struct ast_redir *node)
{
    logger("redir ");
    ast_print(node->number);
    logger("%s", node->pipe);
    logger("%s", node->file);
}
