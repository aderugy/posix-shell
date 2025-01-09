#include "input.h"

#include <err.h>
#include <stdlib.h>

struct ast_input *ast_parse_input(struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    struct ast_list *root = NULL;
    switch (tok.type)
    {
    case TOKEN_EOF:
    case TOKEN_NEW_LINE:
        return NULL;
    default:
        tok = lexer_pop(lexer);
        root = parse_list(lexer);
        // if root == NULL then program has already exited
        // no need to exit
        break;
    }
    tok = lexer_pop(lexer);
    if (tok.type != TOKEN_EOF && tok.type != TOKEN_NEW_LINE)
    {
        return errx("Unexpected token at the end of input parsing");
    }
    return root;
}
int ast_eval_input(__attribute((unused)) struct ast_input *node,
                   __attribute((unused)) void **out)
{
    errx(EXIT_FAILURE, "not implemented");
}
void ast_free_input(__attribute((unused)) struct ast_input *node)
{
    errx(EXIT_FAILURE, "not implemented");
}
void ast_print_input(__attribute((unused)) struct ast_input *input)
{
    errx(EXIT_FAILURE, "not implemented");
}
