#ifndef ELSE_H
#define ELSE_H

#include "lexer/lexer.h"
#include "node.h"

struct ast_else_node
{
    struct ast_node *condition; // clist
    struct ast_node *body; // clist
    struct ast_node *else_clause; // else
};

struct ast_else_node *ast_parse_else(struct lexer *lexer);
int ast_eval_else(struct ast_else_node *node, void **cmd);
void ast_free_else(struct ast_else_node *node);
void ast_print_else(struct ast_else_node *node);

#endif // ELSE_H
