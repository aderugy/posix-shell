#include "element.h"

struct element_node *ast_element_parse(struct lexer *lexer)
{
    struct token *token = lexer_pop(lexer);
}

void ast_element_free(struct element_node *node);
