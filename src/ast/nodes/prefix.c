#define _POSIX_C_SOURCE 200809L

#include "prefix.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "redirection.h"
#include "utils/logger.h"

struct ast_prefix *ast_parse_prefix(struct lexer *lexer)
{
    logger("Parse PREFIX\n");

    struct token *token = lexer_peek(lexer);
    if (!token)
    {
        logger("\tExit PREFIX\n");
        return NULL;
    }

    struct ast_prefix *node = calloc(1, sizeof(struct ast_prefix));
    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    if (token->type == TOKEN_AWORD)
    {
        lexer_pop(lexer);
        node->data = token;
        logger("\tExit PREFIX with AWORD\n");
        return node;
    }

    struct ast_node *redir = ast_create(lexer, AST_REDIRECTION);
    if (!redir)
    {
        free(node);

        logger("Exit PREFIX with NULL\n");
        return NULL;
    }

    node->redir = redir;
    logger("Exit PREFIX with REDIR\n");
    return node;
}

int ast_eval_prefix(struct ast_prefix *node, void **out,
                    struct ast_eval_ctx *ctx)
{
    if (node->data)
    {
        insert(ctx, node->data);
        return EXIT_SUCCESS;
    }
    return ast_eval(node->redir, out, ctx);
}

void ast_free_prefix(struct ast_prefix *node)
{
    if (node)
    {
        token_free(node->data);
        ast_free(node->redir);
        free(node);
    }
}

void ast_print_prefix(struct ast_prefix *node)
{
    ast_print(node->redir);
}
