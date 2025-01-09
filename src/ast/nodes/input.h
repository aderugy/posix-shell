#ifndef INPUT_H
#define INPUT_H

#include "lexer/lexer.h"
#include "node.h"

struct ast_input
{
    int ignored;
};

struct ast_input *ast_parse_input(struct lexer *lexer);
int ast_eval_input(struct ast_input *node, void **out);
void ast_free_input(struct ast_input *node);
void ast_print_input(struct ast_input *input);

#endif // INPUT_H
