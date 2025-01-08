#ifndef CLIST_H
#define CLIST_H

struct ast_node *ast_parse_clist(struct lexer *lexer);
int ast_eval_clist(struct ast_node *node, void **out);
void ast_free_clist(struct ast_node *node);

#endif // !CLIST_H
