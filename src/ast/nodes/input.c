#include "input.h"

#include <err.h>
#include <stdlib.h>

struct ast_input *ast_parse_input(__attribute((unused)) struct lexer *lexer)
{
    errx(EXIT_FAILURE, "not implemented");
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
