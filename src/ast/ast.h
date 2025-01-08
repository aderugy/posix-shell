#ifndef AST_H
#define AST_H
#include <stddef.h>
#include <unistd.h>

#include "lexer/token.h"
#include "nodes/element.h"

#define AST_PARSE_SUCCESS 0
#define AST_PARSE_ERROR 1

enum ast_type
{
    SIMPLE_COMMAND,
    IF,
    LIST,
    AND_OR,
    ELEMENT
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
    struct mbt_str *command_name; // word : name of the command
    struct element_node **elements; // element : list of arguments need to be
                                    // joined during the call of the command
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
struct ast_node *add_node(struct ast_node *node, struct ast_node *new_elt);
struct ast_node *add_element(struct ast_node *ast, struct element_node *child);
void if_node_free(struct if_node *if_node);
void and_or_node_free(struct and_or_node *and_or_node);
void list_node_free(struct list_node *list_node);
void simple_command_node_free(struct simple_command_node *simple_command_node);
void ast_free(struct ast_node *node);

#endif /* ! AST_H */
