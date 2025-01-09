#include "input.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "utils/logger.h"

struct ast_input *ast_parse_input(struct lexer *lexer)
{
    struct token *tok = lexer_peek(lexer);
    struct ast_input *root = NULL;

    switch (tok->type)
    {
    case TOKEN_EOF:
        return NULL;
    case TOKEN_NEW_LINE:
        return NULL;
    default:
        root = calloc(1, sizeof(struct ast_input));

        if (!root)
        {
            errx(EXIT_FAILURE, "out of memory");
        }

        root->list = ast_create(lexer, AST_LIST);
    }
    tok = lexer_pop(lexer);
    if (!tok || (tok->type != TOKEN_EOF && tok->type != TOKEN_NEW_LINE))
    {
        free(tok);
        errx(AST_EVAL_ERROR, "Unexpected token at the end of input parsing");
    }
    logger("eof\n");
    free(tok);
    return root;
}

int ast_eval_input(struct ast_input *node, void **out)
{
    return ast_eval(node->list, out);
}

void ast_free_input(struct ast_input *node)
{
    ast_free(node->list);
    free(node);
}

void ast_print_input(__attribute((unused)) struct ast_input *input)
{
    logger("input\n");
}
