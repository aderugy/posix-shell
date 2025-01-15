#ifndef FOR_H
#define FOR_H

#include "lexer/lexer.h"
#include "node.h"

struct ast_for_node
{
    struct ast_node *elt; // word
    struct ast_node *in; // word
    struct ast_node *body; // clist
};

struct ast_for_node *ast_parse_for(struct lexer *lexer);
int ast_eval_for(struct ast_for_node *node, void **cmd,
                 __attribute((unused)) struct ast_eval_ctx *ctx);
void ast_free_for(struct ast_for_node *node);
void ast_print_for(struct ast_for_node *node);

#endif // FOR_H
