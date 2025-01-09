#include "shell_command.h"

#include <err.h>
#include <stdlib.h>

struct ast_shell_cmd *
ast_parse_shell_cmd(__attribute((unused)) struct lexer *lexer)
{
    errx(EXIT_FAILURE, "not implemented");
}
int ast_eval_shell_cmd(__attribute((unused)) struct ast_shell_cmd *cmd,
                       __attribute((unused)) void **ptr)
{
    errx(EXIT_FAILURE, "not implemented");
}
void ast_free_shell_cmd(__attribute((unused)) struct ast_shell_cmd *cmd)
{
    errx(EXIT_FAILURE, "not implemented");
}
void ast_print_shell_cmd(__attribute((unused)) struct ast_shell_cmd *cmd)
{
    errx(EXIT_FAILURE, "not implemented");
}
