
#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "utils/logger.h"
static struct option l_opts[] = { { "f", no_argument, 0, 'f' },
                                  { "v", no_argument, 0, 'v' },
                                  { 0, 0, 0, 0 } };

int unset_builtin(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx)
{
    int c;
    int opt_idx = 0;
    optind = 1;
    bool fct = false;
    bool var = false;
    while ((c = getopt_long(argc, argv, "fv", l_opts, &opt_idx)) != -1)
    {
        switch (c)
        {
        case 'v':
            var = true;
            break;
        case 'f':
            fct = true;
            break;
        case '?':
        default:
            errx(1, "unset.c : error");
        }
    }

    char *name = argv[optind];

    logger("unset %s\n", name);
    if (var && !hash_map_remove(ast_eval_ctx->value, name, HASH_VARIABLE))
    {
        logger("var not unset %s\n", name);
        return 1;
    }
    else if (fct && !hash_map_remove(ast_eval_ctx->value, name, HASH_FUNCTION))
    {
        logger("var not unset %s\n", name);
        return 1;
    }
    else if (!var && !fct)
    {
        logger("nothing was specified\n");
        if (hash_map_remove(ast_eval_ctx->value, name, HASH_FUNCTION))
        {
            return 0;
        }
        else if (hash_map_remove(ast_eval_ctx->value, name, HASH_VARIABLE))
        {
            logger("is a var and that was not specified");
            return 0;
        }
        else
        {
            return unsetenv(name);
        }
    }
    return 0;
}
