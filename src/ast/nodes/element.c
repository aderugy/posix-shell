#include "element.h"

#include "lexer/token.h"
#include <err.h>
struct element_node *ast_element_parse(struct lexer *lexer)
{
    struct token *token = lexer_pop(lexer);
    if (!token || token->type != TOKEN_WORD)
    {
        warnx("Syntax error");
        return NULL;
    }
}

void ast_element_free(struct element_node *node);
