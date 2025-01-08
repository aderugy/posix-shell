#ifndef AST_ELEMENT_H
#define AST_ELEMENT_H

#include "lexer/lexer.h"
#include "node.h"

struct ast_element_node // can be produced with new_element_node
{
    char *value; // a word that will be an argument
};

struct ast_node *ast_parse_element(struct lexer *lexer);
int ast_eval_element(struct ast_node *node, void **out);
void ast_free_element(struct ast_node *node);

#endif // !AST_ELEMENT_H
