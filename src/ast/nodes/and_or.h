#ifndef AND_OR_H
#define AND_OR_H

#include <stdbool.h>

#include "lexer/lexer.h"

struct ast_and_or_node
{
    bool is_and;
    struct ast_node *left; // unkwown
    struct ast_node *right; // unkwown
};

struct ast_and_or_node *ast_parse_and_or(struct lexer *lexer);
int ast_eval_and_or(struct ast_and_or_node *node, void **out);
void ast_free_and_or(struct ast_and_or_node *and_or_node);
void ast_print_and_or(struct ast_and_or_node *node);

#endif // !#ifndef AND_OR_H
