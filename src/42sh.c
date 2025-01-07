#include <err.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "utils/fill_input.h"
#include "utils/logger.h"

static struct option l_opts[] = { { "verbose", no_argument, 0, 'v' },
                                  { "comput", no_argument, 0, 'c' },
                                  { 0, 0, 0, 0 } };

int main(int argc, char *argv[])
{
    char *input = calloc(1, sizeof(char));
    int c;
    int opt_idx = 0;
    while ((c = getopt_long(argc, argv, "v::c:", l_opts, &opt_idx)) != -1)
    {
        switch (c)
        {
        case 'v':
            logger("Test %s\n", "hello");
            logger(NULL, NULL);
            logger("Test %s\n", "hello");
            break;
        case 'c':
            fill_input_with_argv(argv, argc, optind, input);
            logger("%s", input);
            break;
        case '?':
            exit(1);

        default:
            errx(1, "main: unkown option %c", c);
        }
    }

    return 0;
}
