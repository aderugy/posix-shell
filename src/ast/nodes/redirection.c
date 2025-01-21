#include "redirection.h"

#include <ctype.h>
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
#include "redirection_stdin.h"
#include "redirection_stdout.h"
#include "utils/logger.h"
#include "utils/xalloc.h"

/*
 * redirection = [IONUMBER] ( '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' )
 * WORD ;
 */

int redir_fopen_rw(struct ast_redir *redir, __attribute((unused)) void **out,
                   __attribute((unused)) struct ast_eval_ctx *ctx);

static const struct redirection REDIR_LIST[] = {
    { TOKEN_REDIR_STDOUT_FILE, redir_stdout_file, ">" },
    { TOKEN_REDIR_STDOUT_FILE_A, redir_stdout_file_a, ">>" },
    { TOKEN_REDIR_STDOUT_FILE_NOTRUNC, redir_stdout_file_notrunc, ">|" },
    { TOKEN_REDIR_FILE_STDIN, redir_file_stdin, "<" },
    { TOKEN_REDIR_STDIN_FD, redir_stdin_fd, "<&" },
    { TOKEN_REDIR_FOPEN_RW, redir_fopen_rw, "<>" },
    { TOKEN_REDIR_STDOUT_FD, redir_stdout_fd, ">&" }
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
    struct token *token = lexer_peek(lexer);
    if (!token || !is_redir(token))
    {
        return NULL;
    }

    struct ast_redir *redir = xcalloc(1, sizeof(struct ast_redir));
    logger("PARSE REDIR\n");

    redir->number = -1;
    char number = 1;
    size_t i = 0;
    if (isdigit(*token->value.c))
    {
        number = *token->value.c - '0';
    }

    if (number == 1 && i > 0)
    {
        redir->number = atoi(token->value.c);
    }
    redir->pipe = token->type;
    token_free(lexer_pop(lexer));

    token = lexer_peek(lexer);
    if (!token || token->type != TOKEN_WORD)
    {
        goto error;
    }

    redir->file = strdup(token->value.c);
    token_free(lexer_pop(lexer));

    logger("PARSE REDIR (SUCCESS)\n");
    return redir;

error:
    if (redir)
    {
        logger("Exit REDIR (ERROR)\n");
        ast_free_redir(redir);
    }

    return NULL;
}

/*
 * @MEHDI @JULES
 * Enlever ces errx
 */
int redir_fopen_rw(struct ast_redir *node,
                   __attribute((unused)) struct linked_list *out,
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

    int fd = open(file, O_RDWR);
    if (fd == -1)
    {
        errx(EXIT_FAILURE, "eval_redir: no such file: %s", node->file);
    }

    if (dup2(fd, fd2) == -1)
    {
        errx(2, "redir_eval: dup: error");
    }
    if (out)
    {
        int *origin_fd = *out;
        *origin_fd = fd;
        *(origin_fd + 1) = fd2; // smiley deux yeux grand ouverts
        // Les crochets ont été inventés en 2026. Jules en 2025:
        *(origin_fd + 2) = saved_stdout;
        // Blague rendue possible grace a GIT BLAME
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
    if (node->number != -1)
    {
        logger("%i ", node->number);
    }
    if (node->pipe)
    {
        for (size_t i = 0; i < REDIR_LEN; i++)
        {
            if (REDIR_LIST[i].type == node->pipe)
            {
                logger("%s ", REDIR_LIST[i].token);
                break;
            }
        }
    }
    if (node->file)
    {
        logger("%s", node->file);
    }
}
