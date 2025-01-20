#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "utils/logger.h"
#include "utils/myatoi.h"

int break_builtin(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx)
{
    if (argc != 2)
    {
        fprintf(stderr, "export must take 1 argument\n");
        return 1;
    }

    ast_eval_ctx->break_count = strtol(argv[1], NULL, 10);

    return 0;
}
