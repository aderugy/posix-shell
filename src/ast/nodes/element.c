#include "element.h"

#include <err.h>
#include <stdlib.h>

#include "lexer/token.h"
#include "node.h"

struct ast_element_node *ast_parse_element(struct lexer *lexer)
{
    struct token *token = lexer_pop(lexer);
    if (!token || token->type != TOKEN_WORD)
    {
        return NULL;
    }

    struct ast_element_node *el = calloc(1, sizeof(struct ast_element_node));
    if (!el)
    {
        errx(EXIT_FAILURE, "out of memory");
    }
    el->value = token->value.c;

    return el;
}

int ast_eval_element(struct ast_element_node *node)
{
    return AST_EVAL_SUCCESS;
}

void ast_free_element(struct ast_element_node *node)
{
    free(node->value);
    free(node);
}
