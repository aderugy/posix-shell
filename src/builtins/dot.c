#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "utils/logger.h"

int dot(__attribute__((unused)) int argc, __attribute__((unused)) char **argv,
        __attribute__((unused)) struct ast_eval_ctx *ast_eval_ctx)
{
    return 0;
}
