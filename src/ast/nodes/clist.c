#include "clist.h"

#include <err.h>
#include <stdlib.h>

struct ast_clist *ast_parse_clist(struct lexer *lexer);
int ast_eval_clist(struct ast_clist *node, void **out);
void ast_free_clist(struct ast_clist *node);
void ast_print_clist(struct ast_clist *node);
