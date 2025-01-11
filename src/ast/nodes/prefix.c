#include "prefix.h"

#include <err.h>
#include <stdlib.h>

#include "node.h"
#include "redirection.h"

struct ast_prefix *ast_parse_prefix(struct lexer *lexer)
{
    struct ast_prefix *node = calloc(1, sizeof(struct ast_prefix));
    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    struct ast_node *redir = ast_create(lexer, AST_PREFIX);
    if (!redir)
    {
        free(node);
        return NULL;
    }

    node->redir = redir;
    return node;
}

int ast_eval_prefix(struct ast_prefix *node, void **out)
{
    return ast_eval(node->redir, out);
}

void ast_free_prefix(struct ast_prefix *node)
{
    ast_free(node->redir);
    free(node);
}

void ast_print_prefix(struct ast_prefix *node)
{
    ast_print(node->redir);
}
