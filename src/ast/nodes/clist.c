#include "clist.h"

#include <err.h>
#include <stdlib.h>

struct ast_clist *ast_parse_clist(__attribute((unused)) struct lexer *lexer)
{
    errx(EXIT_FAILURE, "not implemented");
}
int ast_eval_clist(__attribute((unused)) struct ast_clist *node,
                   __attribute((unused)) void **out)
{
    errx(EXIT_FAILURE, "not implemented");
}
void ast_free_clist(__attribute((unused)) struct ast_clist *node)
{
    errx(EXIT_FAILURE, "not implemented");
}
void ast_print_clist(__attribute((unused)) struct ast_clist *node)
{
    errx(EXIT_FAILURE, "not implemented");
}
