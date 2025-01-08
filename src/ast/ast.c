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

struct ast_node *new_list_ast(struct ast_node *new)
{
    struct list_node *value = calloc(1, sizeof(struct list_node));
    if (!value)
    {
        logger("FAILED : not enough memory to calloc a new ast");
        free(new);
        return NULL;
    }
    value->list = calloc(1, sizeof(struct ast_node **));
    value->list[0] = calloc(1, sizeof(struct ast_node *));
    new->value.list_node = value;
    return new;
}

struct list_node *add_node(struct list_node *node, struct ast_node *child)
{
    if (!node->list)
        logger("FAILURE : node's list is not defined");
    node->list =
        realloc(node->list, node->size + 2); // size + new node + NULL BYTE

    node->list[node->size] = child;
    node->size++;
    node->list[node->size] = 0;
    return node;
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

void print_ast()
{
    printf("ast !!!\n");
}

void ast_free(__attribute__((unused)) struct ast_node *node)
{}
