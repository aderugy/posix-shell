#ifndef AST_ELEMENT_H
#define AST_ELEMENT_H

#include "lexer/lexer.h"
#include "node.h"

struct ast_element_node // can be produced with new_element_node
{
    char *value; // a word that will be an argument
};

struct ast_node *ast_element_parse(struct lexer *lexer);
int ast_element_eval(struct ast_node *node);
void ast_element_free(struct ast_node *node);

#endif // !AST_ELEMENT_H
