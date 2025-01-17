#ifndef WHILE_H
#define WHILE_H

#include "lexer/lexer.h"
#include "node.h"

struct ast_while_node
{
    struct ast_node *condition; // clist
    struct ast_node *body; // clist
};

struct ast_while_node *ast_parse_while(struct lexer *lexer);
int ast_eval_while(struct ast_while_node *node, void **cmd,
                   __attribute((unused)) struct ast_eval_ctx *ctx);
void ast_free_while(struct ast_while_node *node);
void ast_print_while(struct ast_while_node *node);

#endif // WHILE_H
