#ifndef CLIST_H
#define CLIST_H

#include "lexer/lexer.h"
#include "node.h"

struct ast_clist
{
    struct linked_list *list;
};

struct ast_clist *ast_parse_clist(struct lexer *lexer);
int ast_eval_clist(struct ast_clist *node, struct linked_list *out,
                   struct ast_eval_ctx *ctx);
void ast_free_clist(struct ast_clist *node);
void ast_print_clist(struct ast_clist *node);

#endif // !CLIST_H
