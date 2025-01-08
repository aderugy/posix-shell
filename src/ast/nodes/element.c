#include "element.h"

#include <err.h>
#include <stdlib.h>

#include "ast/ast.h"
#include "lexer/token.h"

struct ast_node *ast_element_parse(struct lexer *lexer)
{
    struct token *token = lexer_pop(lexer);
    if (!token || token->type != TOKEN_WORD)
    {
        warnx("Syntax error");
        return NULL;
    }

    struct ast_node *node = calloc(1, sizeof(struct ast_node));
    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    struct ast_element_node *el = calloc(1, sizeof(struct ast_element_node));
    if (!el)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    el->value = token->value.c;
}

void ast_element_free(struct element_node *node);
