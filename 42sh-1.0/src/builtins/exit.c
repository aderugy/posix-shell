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

int exit_builtin(int argc, char **argv)
{
    if (argc == 1)
    {
        exit(0);
    }
    if (argc != 2)
        errx(1, "exit: too many arguments");
    if (is_valid_number(argv[1]) == 0)
        errx(2, "exit: a: numeric argument required");
    exit(atoi(argv[1]) % 255);
}
