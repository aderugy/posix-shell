#include "list.h"

#include <err.h>
#include <stdlib.h>

#include "node.h"

struct ast_list *ast_parse_list(struct lexer *lexer)
{
    struct ast_node *and_or = ast_create(lexer, AST_AND_OR);
    if (!and_or)
    {
        return NULL;
    }

    struct ast_list *node = calloc(1, sizeof(struct ast_list));
    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    node->list = list_init();
    list_append(node->list, and_or);

    return node;
}

int ast_eval_list(struct ast_list *node, void **out);

void ast_free_list(struct ast_list *node)
{
    list_free(node->list, ast_free);
    free(node);
}
