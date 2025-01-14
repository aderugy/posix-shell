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
    logger("Parse REDIRECTION\n");
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
    redir->pipe = token->value.c;

    free(lexer_pop(lexer));
    token = lexer_peek(lexer);
    if (!token || token->type != TOKEN_WORD)
    {
        token = NULL;
        goto error;
    }

    redir->number = number;
    redir->file = token->value.c;
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
    logger("Exit REDIRECTION\n");
    return NULL;
}

int redir_file_stdin(struct ast_redir *node)
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

int redir_stdout_file(struct ast_redir *node)
{
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
    if (fd == -1)
    {
        errx(EXIT_FAILURE, "eval_redir: no such file: %s", node->file);
    }
    if (dup2(fd, fd2) == -1)
        errx(2, "redir_eval: dup: error");
    logger("Exit Eval Redir\n");
    return 0;
}

int ast_eval_redir(struct ast_redir *node, __attribute((unused)) void **out)
{
    logger("Eval redir\n");
    return redir_stdout_file(node);
}

void ast_free_redir(struct ast_redir *node)
{
    free(node->file);
    free(node->pipe);
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
