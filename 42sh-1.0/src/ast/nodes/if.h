#ifndef IF_H
#define IF_H

#include "lexer/lexer.h"
#include "node.h"

struct ast_if_node
{
    struct ast_node *condition; // clist
    struct ast_node *body; // clist
    struct ast_node *else_clause; // else
};

struct ast_if_node *ast_parse_if(struct lexer *lexer);
int ast_eval_if(struct ast_if_node *node, void **cmd,
                __attribute((unused)) struct ast_eval_ctx *ctx);
void ast_free_if(struct ast_if_node *node);
void ast_print_if(struct ast_if_node *node);

#endif // IF_H
