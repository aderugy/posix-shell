#include "node.h"

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
