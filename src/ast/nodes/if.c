#include "if.h"

#include <err.h>
#include <stdlib.h>

#include "lexer/lexer.h"
#include "node.h"

struct ast_node *ast_parse_if(struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type != TOKEN_IF)
    {
        return NULL;
    }

    lexer_pop(lexer);

    struct ast_if_node *node = calloc(1, sizeof(struct ast_if_node));
    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    // node->condition = ast_create(CLIST)
    // check null -> error
    // lexer_pop: expect then
    // check then
    // node->body = ast_create(clist)
    // check null
    // lexer_peek
    //      1. else | elif -> ast_create(else | elif)
    // lexer_pop: expect fi
    // check

    errx(EXIT_FAILURE, "not implemented");
    return NULL;
}
