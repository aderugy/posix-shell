#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "commands.h"
#include "utils/logger.h"

int break_builtin(__attribute__((unused)) int argc, char **argv,
                  struct ast_eval_ctx *ast_eval_ctx)
{
    if (argc > 1)
    {
        ast_eval_ctx->break_count = strtol(argv[1], NULL, 10);
    }
    else
    {
        ast_eval_ctx->break_count++;
    }

    return 0;
}
