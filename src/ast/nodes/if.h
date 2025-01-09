#ifndef IF_H
#define IF_H

#include "lexer/lexer.h"
#include "node.h"

struct if_node
{
    struct ast_node *condition; // unkwown
    struct ast_node *body; // unkwown
    struct ast_node *else_clause; // unknown
};

struct ast_if_node *ast_parse_if(struct lexer *lexer);
int ast_eval_if(struct ast_if_node *node, void **cmd);
void ast_free_if(struct ast_if_node *node);
void ast_print_if(struct ast_if *node);

#endif // IF_H
