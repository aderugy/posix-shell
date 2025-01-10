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

static const char *REDIRECTIONS[] = { ">",  "<",  ">>", ">&",
                                      "<&", ">,", "<>", NULL };

static int is_redir(char *s)
{
    for (int i = 0; REDIRECTIONS[i]; i++)
    {
        if (strcmp(s, REDIRECTIONS[i]) == 0)
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

    struct ast_node *element = NULL;
    struct ast_node *number = ast_create(lexer, AST_IONUMBER);
    struct token *token = lexer_peek(lexer);

    // Note: might need to push back the IO Number on the stream if it fails
    if (!token || token->type != TOKEN_WORD || !is_redir(token->value.c))
    {
        goto error;
    }

    // Note: Fix element -> just simple word
    element = ast_create(lexer, AST_ELEMENT);
    if (!element)
    {
        goto error;
    }

    redir->number = number;
    redir->pipe = token->value.c;
    redir->word = element;
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
    if (token)
    {
        free(token);
    }
    if (element)
    {
        ast_free(element);
    }
    return NULL;
}

int ast_eval_redir(__attribute((unused)) struct ast_redir *node)
{
    errx(EXIT_FAILURE, "not implemented");
}

void ast_free_redir(struct ast_redir *node)
{
    ast_free(node->word);
    free(node->pipe);
    ast_free(node->number);
    free(node);
}

void ast_print_redir(struct ast_redir *node)
{
    logger("redir ");
    ast_print(node->number);
    logger("%s", node->pipe);
    ast_print(node->word);
}
