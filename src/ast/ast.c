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
    value->list = calloc(1, sizeof(struct ast_node **));
    value->list[0] = calloc(1, sizeof(struct ast_node *));
    new->value.list_node = value;
    return new;
}

struct ast_node *add_node(struct ast_node *ast, struct ast_node *child)
{
    struct list_node *node = ast->value.list_node;
    node->list =
        realloc(node->list, sizeof(struct ast_node *) * (node->size + 2));
    // size + new node + NULL

    node->list[node->size] = child;
    node->size++;
    node->list[node->size] = NULL;

    return ast;
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

void simple_command_node_free(struct simple_command_node *simple_command_node)
{
    mbt_str_free(simple_command_node->command_name);
    for (size_t i = 0; i < simple_command_node->elements_len; i++)
    {
        free((simple_command_node->elements)[i]);
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

void and_or_node_free(struct and_or_node *and_or_node)
{
    ast_free(and_or_node->left);
    ast_free(and_or_node->right);
    free(and_or_node);
}

void list_node_free(struct list_node *list_node)
{
    for (size_t i = 0; (list_node->list)[i] != NULL; i++)
    {
        ast_free((list_node->list)[i]);
    }
    free(list_node);
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
