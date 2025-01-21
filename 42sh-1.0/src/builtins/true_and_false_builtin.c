#include "commands.h"

int true_builtin(__attribute__((unused)) int argc,
                 __attribute__((unused)) char **argv,
                 __attribute__((unused)) struct ast_eval_ctx *ast_eval_ctx)
{
    return 0;
}

int false_builtin(__attribute__((unused)) int argc,
                  __attribute__((unused)) char **argv,
                  __attribute__((unused)) struct ast_eval_ctx *ast_eval_ctx)
{
    return 1;
}
