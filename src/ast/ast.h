#ifndef AST_H
#define AST_H
#include <stddef.h>
#include <unistd.h>

#include "lexer/token.h"

enum ast_type
{
    SIMPLE_COMMAND,
    IF,
    SHELL_COMMAND,
    PIPELINE,
    AND_OR,
    LIST,
};

enum and_or_type
{
    AND,
    OR
};
struct and_or_node
{
    struct ast_node *left; // unkwown
    struct ast_node *right; // unkwown
    enum and_or_type type;
};
struct list_node
{
    struct ast_node **list; // and_or
    size_t size;
};
struct if_node
{
    struct ast_node *condition; // unkwown
    struct ast_node *body; // unkwown
    struct ast_node *else_clause; // unknown
};
// has to be a leaf node !
struct simple_command_node
{
    struct mbt_str *command_name;
    struct mbt_str **elements;
    size_t elements_len;
};

struct ast_node
{
    enum ast_type type;
    union
    {
        struct simple_command_node *simple_command;
        struct if_node *if_node;
        struct and_or_node *and_or_node;
        struct list_node *list_node;
    } value;
};

struct ast_node *new_ast(enum ast_type type);
struct list_node *add_node(struct list_node *node, struct ast_node *child);
void ast_free(struct ast_node *node);
void print_ast();

#endif /* ! AST_H */
