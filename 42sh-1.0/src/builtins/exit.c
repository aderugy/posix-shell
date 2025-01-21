#include <err.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "builtins.h"
#include "commands.h"

int my_isdigit(char c)
{
    return (c >= '0' && c <= '9');
}

static int is_valid_number(const char *str)
{
    while (*str)
    {
        if (!my_isdigit(*str))
            return 0;
        str++;
    }
    return 1;
}

int exit_builtin(int argc, char **argv,
                 __attribute__((unused)) struct ast_eval_ctx *ast_eval_ctx)
{
    if (argc == 1)
    {
        exit(0);
    }
    if (argc != 2)
    {
        warnx("exit: too many arguments");
        return 1;
    }
    if (is_valid_number(argv[1]) == 0)
    {
        warnx("exit: a: numeric argument required");
        return 2;
    }
    exit(atoi(argv[1]) % 255);
}
