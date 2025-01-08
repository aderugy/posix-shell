#include "and_or.h"

#include <err.h>
#include <stdlib.h>

#include "lexer/token.h"
#include "node.h"
#include "utils/logger.h"

struct ast_node *new_and_or_ast(struct ast_node *new)
{
    struct and_or_node *value = calloc(1, sizeof(struct and_or_node));
    if (!value)
    {
        logger("FAILED : not enough memory to calloc a new ast");
        free(new);
        return NULL;
    }
    new->value.and_or_node = value;
    return new;
}
void and_or_node_free(struct and_or_node *and_or_node)
{
    ast_free(and_or_node->left);
    ast_free(and_or_node->right);
    free(and_or_node);
}

struct ast_node *parse_and_or(struct lexer *lexer)
{
    return parse_pipeline(lexer);
}
