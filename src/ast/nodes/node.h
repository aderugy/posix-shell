#ifndef AST_NODE_H
#define AST_NODE_H

#include "lexer/lexer.h"

#define AST_EVAL_SUCCESS 0
#define AST_EVAL_ERROR 1

enum ast_type
{
    AST_SIMPLE_COMMAND,
    AST_SHELL_COMMAND,
    AST_PIPELINE,
    AST_COMMAND,
    AST_ELEMENT,
    AST_AND_OR,
    AST_CLIST,
    AST_INPUT,
    AST_LIST,
    AST_IF,
    AST_ELSE
};

struct ast_node
{
    enum ast_type type;
    void *value;
};

struct ast_node_operations
{
    void *(*parse)(struct lexer *lexer);
    int (*eval)(void *node, void **out);
    void (*free)(void *node);
    void (*print)(void *node);
};

struct ast_node *ast_create(struct lexer *lexer, enum ast_type type);
int ast_eval(struct ast_node *node, void **out);
void ast_free(struct ast_node *node);
void ast_print(struct ast_node *node);

#endif // !AST_NODE_H
