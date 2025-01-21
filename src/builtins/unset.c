
#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "utils/logger.h"

int unset_builtin(int argc, char **argv,
                  __attribute__((unused)) struct ast_eval_ctx *ast_eval_ctx)
{
    if (argc != 2)
    {
        fprintf(stderr, "export must take 1 argument\n");
        return 1;
    }

    char *name = argv[1];

    logger("unset %s\n", name);

    return unsetenv(name);
}
