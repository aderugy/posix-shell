#ifndef AST_NODE_H
#define AST_NODE_H

#include "lexer/lexer.h"

#define AST_EVAL_SUCCESS 0
#define AST_EVAL_ERROR 1

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

struct ast_node
{
    enum ast_type type;
    void *value;
};

struct ast_node *ast_create(struct lexer *lexer, enum ast_type type);
void ast_free(struct ast_node *node);

#endif // !AST_NODE_H
