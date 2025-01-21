#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "utils/logger.h"

int export_builtin(int argc, char **argv,
                   __attribute__((unused)) struct ast_eval_ctx *ast_eval_ctx)
{
    if (argc != 2)
    {
        fprintf(stderr, "export must take 1 argument\n");
        return 1;
    }

    char *name = argv[1];

    char *equal_sign = strchr(argv[1], '=');
    char *word = NULL;
    if (!equal_sign)
    {
        struct mbt_str *n = mbt_str_init(8);
        mbt_str_pushcstr(n, name);
        word = ctx_get_value(ast_eval_ctx, n);
    }
    else
    {
        word = equal_sign + 1;

        *equal_sign = 0;
    }

    logger("setenv %s=%s\n", name, word);

    if (word)
    {
        return setenv(name, word, 1);
    }
    return 1;
}
