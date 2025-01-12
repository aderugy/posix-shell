#ifndef ELEMENT_H
#define ELEMENT_H

#include "lexer/lexer.h"
#include "node.h"

struct ast_element // can be produced with new_element_node
{
    char *value; // a word that will be an argument
    struct ast_node *redir;
};

struct ast_element *ast_parse_element(struct lexer *lexer);
int ast_eval_element(struct ast_element *node, void **out);
void ast_free_element(struct ast_element *node);
void ast_print_element(struct ast_element *node);

#endif // !ELEMENT_H
