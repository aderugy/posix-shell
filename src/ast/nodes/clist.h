#ifndef CLIST_H
#define CLIST_H

#include "lexer/lexer.h"

struct ast_clist
{};

struct ast_clist *ast_parse_clist(struct lexer *lexer);
int ast_eval_clist(struct ast_clist *node, void **out);
void ast_free_clist(struct ast_clist *node);

#endif // !CLIST_H
