#include "element.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "mbtstr/str.h"
#include "node.h"
#include "utils/logger.h"

/**
 * element = WORD
 * | redirection ;
 */

struct ast_element *ast_parse_element(struct lexer *lexer)
{
    logger("\tParse ELEMENT\n");
    struct token *token = lexer_peek(lexer); // We check if the token is valid
    if (!token)
    {
        logger("\tExit ELEMENT\n");
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
        free(token->state);
        free(token);
        logger("\tExit ELEMENT\n");
        return node;
    }
    char *word = NULL;
    if (word != NULL) // wtf c'est quoi ça ?
    {
        lexer_pop(lexer);
        node->value = word;
        free(token->state);
        free(token);
        return node;
    }

    struct ast_node *redir = ast_create(lexer, AST_REDIRECTION);
    if (!redir)
    {
        ast_free_element(node);
        logger("\tExit ELEMENT\n");
        return NULL;
    }

    node->redir = redir;
    logger("\tExit ELEMENT\n");
    return node;
}

int ast_eval_element(struct ast_element *node, void **out,
                     __attribute((unused)) struct ast_eval_ctx *ctx)
{
    if (node->redir)
    {
        if (node->child == 0)
        {
            node->child = 1;
            return 1;
        }
        ast_eval(node->redir, out, NULL);
        return 1;
    }
    else
    {
        if (node->child == 1)
        {
            return 0;
        }
        node->child = 1;

        *out = node->value;
        return 0;
    }

    return 0;
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
