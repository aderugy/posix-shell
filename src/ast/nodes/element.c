#include "element.h"

#include <err.h>
#include <stdlib.h>

#include "lexer/token.h"
#include "node.h"
#include "utils/logger.h"

/**
 * element = WORD | redirection ;
 */

struct ast_element *ast_parse_element(struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer); // We check if the token is valid
    if (!token)
    {
        return NULL;
    }

    struct ast_element *node = calloc(1, sizeof(struct ast_element));
    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    if (token->type == TOKEN_WORD)
    {
        lexer_pop(lexer); // Valid token -> we consume it

        node->value = token->value.c;
        free(token);
        return node;
    }

    struct ast_node *redir = ast_create(lexer, AST_REDIRECTION);
    if (!redir)
    {
        free(node);
        return NULL;
    }

    node->redir = redir;
    return node;
}

int ast_eval_element(struct ast_element *node, void **out)
{
    *out = node->value;

    if (node->redir)
    {
        ast_eval(node->redir, NULL);
    }

    return AST_EVAL_SUCCESS;
}

void ast_free_element(struct ast_element *node)
{
    if (node->value)
    {
        free(node->value);
    }
    if (node->redir)
    {
        ast_free(node->redir);
    }

    free(node);
}

void ast_print_element(__attribute((unused)) struct ast_element *node)
{
    if (node->value)
    {
        logger("%s", node->value);
    }
    if (node->redir)
    {
        ast_print(node->redir);
    }
}
