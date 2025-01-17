#include "element.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "expansion/expansion.h"
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

    struct ast_node *redir = ast_create(lexer, AST_REDIRECTION);
    if (redir)
    {
        node->redir = redir;
        return node;
    }

    if (token->type == TOKEN_WORD || token->type == TOKEN_AWORD)
    {
        // Valid token -> we consume it
        lexer_pop(lexer);

        node->token = token;
        logger("\tExit ELEMENT\n");
        return node;
    }

    ast_free_element(node);
    logger("\tExit ELEMENT\n");
    return NULL;
}

int ast_eval_element(struct ast_element *node, void **out,
                     struct ast_eval_ctx *ctx)
{
    if (node->token)
    {
        if (node->child == 1)
        {
            return 0;
        }
        node->child = 1;
        struct mbt_str *str = expand(ctx, node->token);

        // Recycling the node->expanded
        free(node->expanded);
        node->expanded = strdup(str->data);
        *out = node->expanded;

        mbt_str_free(str);

        return 0;
    }
    else if (node->redir)
    {
        if (node->child == 0)
        {
            node->child = 1;
            return 1;
        }
        ast_eval(node->redir, out, ctx);
        return 1;
    }
    else
    {
        if (node->child == 1)
        {
            return 0;
        }
        node->child = 1;

        *out = node->token->value.c;
        return 0;
    }

    return 0;
}

void ast_free_element(struct ast_element *node)
{
    if (node)
    {
        if (node->expanded)
        {
            free(node->expanded);
        }
        if (node->redir)
        {
            ast_free(node->redir);
        }
        if (node->token)
        {
            token_free(node->token);
        }

        free(node);
    }
}

void ast_print_element(struct ast_element *node)
{
    if (node->value)
    {
        logger("%s", node->value);
    }
    if (node->redir)
    {
        ast_print(node->redir);
    }
    /*
    if (node->token)
    {
        logger("%s", node->token->value.c);
    }
    */
}
