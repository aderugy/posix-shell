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

int redir_fopen_rw(struct ast_redir *redir,
                   __attribute((unused)) struct linked_list *out,
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

    redir->file = ast_create(lexer, AST_COMPLEX_WORD);
    logger("PARSE REDIR (SUCCESS)\n");
    return redir;
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

    int fd = open(file, O_RDWR);
    if (fd == -1)
    {
        errx(EXIT_FAILURE, "eval_redir: no such file: %s", file);
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

    list_free(filenames, (void (*)(void *))eval_output_free);
    return AST_EVAL_SUCCESS;

error:
    list_free(filenames, (void (*)(void *))eval_output_free);
    return AST_EVAL_ERROR;
}

int ast_eval_redir(struct ast_redir *node, struct linked_list *out,
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
    if (node->file)
    {
        ast_free(node->file);
    }
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
        ast_print(node->file);
    }
}
