#include "redirection.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

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
    return NULL;
}

int ast_eval_redir(__attribute((unused)) struct ast_redir *node,
                   __attribute((unused)) void **out)
{
    errx(EXIT_FAILURE, "not implemented");
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
