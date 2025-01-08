#ifndef AND_OR_H
#define AND_OR_H

#include "ast/nodes/and_or.h"
#include "node.h"

struct and_or_node
{
    struct ast_node *left; // unkwown
    struct ast_node *right; // unkwown
    enum and_or_type type;
};

struct ast_node *ast_init_and_or(struct ast_node *new);

void ast_free_and_or_node(struct and_or_node *and_or_node);

struct ast_node *ast_parse_and_or(struct lexer *lexer);

#endif // !#ifndef AND_OR_H
