#include "if.h"

#include <err.h>
#include <stdlib.h>

#include "lexer/lexer.h"

struct ast_if_node *ast_parse_if(__attribute((unused)) struct lexer *lexer)
{
    errx(EXIT_FAILURE, "not implemented");
}

int ast_eval_if(__attribute((unused)) struct ast_if_node *node,
                __attribute((unused)) void **out)
{
    errx(EXIT_FAILURE, "not implemented");
}

void ast_free_if(__attribute((unused)) struct ast_if_node *node)
{
    errx(EXIT_FAILURE, "not implemented");
}

void ast_print_if(__attribute((unused)) struct ast_if_node *node)
{
    errx(EXIT_FAILURE, "not implemented");
}
