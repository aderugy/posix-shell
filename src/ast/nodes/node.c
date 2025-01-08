#include "node.h"

#include <stdio.h>
#include <stdlib.h>

#include "utils/logger.h"

struct ast_node *new_ast(enum ast_type type)
{
    struct ast_node *new = calloc(1, sizeof(struct ast_node));
    if (!new)
    {
        logger("FAILED : not enough memory to calloc a new ast");
        return NULL;
    }
    new->type = type;
    switch (type)
    {
    case SIMPLE_COMMAND:
        return new_simple_command_ast(new);

    case IF:
        return new_if_ast(new);

    case AND_OR:
        return new_and_or_ast(new);

    case LIST:
        return new_list_ast(new);

    default:
        logger("FAILURE : NOT KNOWN TYPE");
        free(new);
        return NULL;
    }
    return NULL;
}

void ast_free(struct ast_node *node)
{
    if (node->type == IF)
        if_node_free(node->value.if_node);

    else if (node->type == SIMPLE_COMMAND)
        simple_command_node_free(node->value.simple_command);

    else if (node->type == LIST)
        list_node_free(node->value.list_node);

    else if (node->type == AND_OR)
        and_or_node_free(node->value.and_or_node);

    free(node);
}
