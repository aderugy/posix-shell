#ifndef AST_H
#define AST_H
#include <unistd.h>

#include "lexer/token.h"

enum ast_type
{
    SIMPLE_COMMAND,
    IF,
    SHELL_COMMAND,
    PIPELINE,
    AND_OR,
};

struct if_node
{
    struct ast_node *condition; // unkwown
    struct ast_node *body; // unkwown
    struct ast_node *else_clause; // is a if node
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
    } value;
};

struct ast_node *new_ast(enum ast_type type);
void ast_free(struct ast_node *node);
void print_ast();

#endif /* ! AST_H */
