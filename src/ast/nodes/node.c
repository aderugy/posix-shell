#include "node.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "command.h"
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
    case AST_SIMPLE_COMMAND:
        root->value = ast_parse_simple_cmd(lexer);
        break;
    case AST_IF:
        break;
    case AST_LIST:
        break;
    case AST_AND_OR:
        break;
    case AST_ELEMENT:
        root->value = ast_parse_element(lexer);
        break;
    case AST_CLIST:
        break;
    case AST_INPUT:
        break;
    case AST_PIPELINE:
        break;
    case AST_COMMAND:
        root->value = ast_parse_cmd(lexer);
        break;
    case AST_SHELL_COMMAND:
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
