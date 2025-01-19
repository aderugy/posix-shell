
#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "utils/logger.h"
/*static struct option l_opts[] = { { "p", no_argument, 0, 'p' },
                                  { 0, 0, 0, 0 } };*/
int export_builtin(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "export must take 1 argument\n");
    }
    /*int c;

    int opt_idx = 0;
    optind = 1;
    while ((c = getopt_long(argc, argv, "p", l_opts, &opt_idx)) != -1)
    {
        switch (c)
        {
        case 'p':
            break;
        case '?':
            return 1;
        default:
            errx(1, "echo: unkown option %c", c);
        }
    }*/
    char *name = argv[1];
    char *equal_sign = strchr(argv[1], '=');
    char *word = equal_sign + 1;
    *equal_sign = 0;

    logger("setenv %s=%s\n", name, word);
    setenv(name, word, 0);

    return 0;
}
