#ifndef AST_ELEMENT_H
#define AST_ELEMENT_H

#include "lexer/lexer.h"

struct element_node // can be produced with new_element_node
{
    char *element; // a word that will be an argument
};

struct element_node *ast_element_parse(struct lexer *lexer);
int ast_element_eval() void ast_element_free(struct element_node *node);

#endif // !AST_ELEMENT_H
