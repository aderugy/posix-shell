#ifndef AND_OR_H
#define AND_OR_H

#include "ast/ast.h"
#include "ast/nodes/and_or.h"

struct and_or_node
{
    struct ast_node *left; // unkwown
    struct ast_node *right; // unkwown
    enum and_or_type type;
};

struct ast_node *new_and_or_ast(struct ast_node *new);

void and_or_node_free(struct and_or_node *and_or_node);

struct ast_node *parse_and_or(struct lexer *lexer);

#endif // !#ifndef AND_OR_H
