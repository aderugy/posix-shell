#ifndef INPUT_H
#define INPUT_H

#include "lexer/lexer.h"
#include "node.h"

struct ast_node *ast_parse_input(struct lexer *lexer);
int ast_eval_input(struct ast_node *node, void **out);
void ast_free_input(struct ast_node *node);

#endif // INPUT_H
