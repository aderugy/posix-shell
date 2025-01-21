#ifndef ELEMENT_H
#define ELEMENT_H

#include "lexer/lexer.h"
#include "node.h"

enum ast_element_type
{
    AST_ELEMENT_WORD = 0,
    AST_ELEMENT_REDIR
};

struct ast_element // can be produced with new_element_node
{
    enum ast_element_type type;
    struct ast_node *child;
};

struct ast_element *ast_parse_element(struct lexer *lexer);
int ast_eval_element(struct ast_element *node, void **out,
                     __attribute((unused)) struct ast_eval_ctx *ctx);
void ast_free_element(struct ast_element *node);
void ast_print_element(struct ast_element *node);

#endif // !ELEMENT_H
