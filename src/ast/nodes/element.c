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
#include "utils/xalloc.h"

/**
 * element = WORD
 * | redirection ;
 */

struct ast_element *ast_parse_element(struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer); // We check if the token is valid
    if (!token)
    {
        return NULL;
    }

    struct ast_element *node = xcalloc(1, sizeof(struct ast_element));

    logger("Parse ELEMENT\n");
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
        logger("Exit ELEMENT (SUCCESS)\n");
        return node;
    }

    ast_free_element(node);
    logger("Exit ELEMENT (ERROR)\n");
    return NULL;
}

int ast_eval_element(struct ast_element *node, void **out,
                     struct ast_eval_ctx *ctx)
{
    if (node->token && !ctx->check_redir)
    {
        struct mbt_str *str = expand(ctx, node->token);

        // Recycling the node->expanded
        free(node->expanded);
        node->expanded = strdup(str->data);
        *out = node->expanded;

        mbt_str_free(str);
    }
    else if (node->redir)
    {
        if (ctx->check_redir)
        {
            int redir_eval_result = ast_eval(node->redir, out, ctx);
            if (redir_eval_result != 0)
            {
                return redir_eval_result;
            }
        }
        return -1;
    }
    else if (!ctx->check_redir)
    {
        *out = node->value;
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
