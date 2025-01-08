#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

#include "parser/parser.h"
#include "utils/logger.h"

struct ast_node *new_simple_command_ast(struct ast_node *new)
{
    struct simple_command_node *value =
        calloc(1, sizeof(struct simple_command_node));
    if (!value)
    {
        logger("FAILED : not enough memory to calloc a new ast");
        free(new);
        return NULL;
    }
    new->value.simple_command = value;
    return new;
}

struct ast_node *new_if_ast(struct ast_node *new)
{
    struct if_node *value = calloc(1, sizeof(struct if_node));
    if (!value)
    {
        logger("FAILED : not enough memory to calloc a new ast");
        free(new);
        return NULL;
    }
    new->value.if_node = value;
    return new;
}

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
        break;

    case IF:
        return new_if_ast(new);
        break;

    default:
        logger("FAILURE : NOT KNOWN TYPE");
        free(new);
        return NULL;
    }
    return NULL;
}

void print_ast()
{
    printf("ast !!!\n");
}
