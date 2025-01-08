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

struct ast_if_node *ast_parse_if(struct lexter *lexer);
#endif // IF_H
