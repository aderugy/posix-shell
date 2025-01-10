#ifndef UNTIL_H
#define UNTIL_H

#include "lexer/lexer.h"
#include "node.h"

struct ast_until_node
{
    struct ast_node *condition; // clist
    struct ast_node *body; // clist
};

struct ast_until_node *ast_parse_until(struct lexer *lexer);
int ast_eval_until(struct ast_until_node *node, void **cmd);
void ast_free_until(struct ast_until_node *node);
void ast_print_until(struct ast_until_node *node);

#endif // UNTIL_H
