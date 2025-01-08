#include "node.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "element.h"
#include "utils/logger.h"

struct ast_node *ast_create(struct lexer *lexer, enum ast_type type)
{
    struct ast_node *root = calloc(1, sizeof(struct ast_node));
    if (!root)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    switch (type)
    {
    case SIMPLE_COMMAND:
        break;
    case IF:
        break;
    case LIST:
        break;
    case AND_OR:
        break;
    case ELEMENT:
        root->value = ast_parse_element(lexer);
        break;

    default:
        errx(1, "type is not implemented");
    }
    return root;
}

void ast_free(struct ast_node *node)
{
    switch (node->type)
    {
    case SIMPLE_COMMAND:
        break;
    case IF:
        break;
    case LIST:
        break;
    case AND_OR:
        break;
    case ELEMENT:
        break;

    default:
        errx(EXIT_FAILURE, "type is not implmented");
    }
    free(node);
}
