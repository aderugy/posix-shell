#include "element.h"

#include <err.h>
#include <stdlib.h>

#include "lexer/token.h"
#include "node.h"
#include "utils/logger.h"

struct ast_element *ast_parse_element(struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer); // We check if the token is valid
    if (!token || token->type != TOKEN_WORD)
    {
        return NULL;
    }

    lexer_pop(lexer); // Valid token -> we consume it
    struct ast_element *el = calloc(1, sizeof(struct ast_element));
    if (!el)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    el->value = token->value.c;
    return el;
}

int ast_eval_element(__attribute((unused)) struct ast_element *node,
                     __attribute((unused)) void **out)
{
    *out = node->value;
    return AST_EVAL_SUCCESS;
}

void ast_free_element(struct ast_element *node)
{
    free(node->value);
    free(node);
}

void ast_print_element(__attribute((unused)) struct ast_element *node)
{
    logger(" %s ", node->value);
}
