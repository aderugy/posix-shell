#ifndef COMMAND_H
#define COMMAND_H

#include "lexer/lexer.h"
#include "node.h"

struct ast_node *ast_parse_command(struct lexer *lexer);
int ast_eval_command(struct ast_node *node, void **out);
void ast_free_command(struct ast_node *node);

#endif // !COMMAND_H
