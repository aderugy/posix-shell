#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

#include "mbtstr/str.h"
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
    new->value.list_node = value;
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

void simple_command_node_free(struct simple_command_node *simple_command_node)
{
    mbt_str_free(simple_command_node->command_name);
    for (size_t i = 0; i < simple_command_node->elements_len; i++)
    {
        mbt_str_free((simple_command_node->elements)[i]);
    }
    free(simple_command_node->elements);
}

void if_node_free(struct if_node *if_node)
{
    ast_free(if_node->condition);
    ast_free(if_node->body);
    ast_free(if_node->else_clause);
    free(if_node);
}

void ast_free(struct ast_node *node)
{
    if (node->type == IF)
    {
        if_node_free(node->value.if_node);
    }
    else if (node->type == SIMPLE_COMMAND)
    {
        simple_command_node_free(node->value.simple_command);
    }

    free(node);
}
