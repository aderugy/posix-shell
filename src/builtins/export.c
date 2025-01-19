
#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "utils/logger.h"

int export_builtin(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "export must take 1 argument\n");
        return 1;
    }

    char *name = argv[1];

    char *equal_sign = strchr(argv[1], '=');
    char *word;
    if (!equal_sign)
    {
        errx(2,
             "export : faut que j'investigue cette histoire de export sans =");
    }

    word = equal_sign + 1;

    *equal_sign = 0;

    logger("setenv %s=%s\n", name, word);

    return setenv(name, word, 0);
}
