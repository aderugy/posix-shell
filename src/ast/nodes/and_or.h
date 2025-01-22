#ifndef AND_OR_H
#define AND_OR_H

#include <stdbool.h>

#include "lexer/lexer.h"
#include "node.h"

#define AND 0
#define OR 1
#define NONE 2

struct ast_and_or_node
{
    char type;
    struct ast_node *left; // pipeline
    struct ast_and_or_node *right; // and_or
};

struct ast_and_or_node *ast_parse_and_or(struct lexer *lexer);
int ast_eval_and_or(struct ast_and_or_node *node, struct linked_list *out,
                    struct ast_eval_ctx *ctx);
void ast_free_and_or(struct ast_and_or_node *and_or_node);
void ast_print_and_or(struct ast_and_or_node *node);

#endif // !#ifndef AND_OR_H
