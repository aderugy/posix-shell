#include "eval_ctx.h"
#define _POSIX_C_SOURCE 200809L

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "prefix.h"
#include "redirection.h"
#include "utils/logger.h"
#include "utils/xalloc.h"

struct ast_prefix *ast_parse_prefix(struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (!token)
    {
        return NULL;
    }

    struct ast_prefix *node = xcalloc(1, sizeof(struct ast_prefix));

    if (token->type == TOKEN_AWORD)
    {
        lexer_pop(lexer);
        node->data = token;

        logger("PARSE PREFIX (SUCCESS)\n");
        return node;
    }

    struct ast_node *redir = ast_create(lexer, AST_REDIRECTION);
    if (!redir)
    {
        ast_free_prefix(node);
        return NULL;
    }

    node->redir = redir;
    logger("PARSE PREFIX (SUCCESS)\n");
    return node;
}

int ast_eval_prefix(struct ast_prefix *node, void **out,
                    struct ast_eval_ctx *ctx)
{
    if (node->data)
    {
        ctx_insert_value(ctx, node->data);
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
