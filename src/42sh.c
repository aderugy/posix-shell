#include <err.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins/builtins.h"
#include "utils/fill_input.h"
#include "utils/logger.h"
#include "utils/test_lexer.h"
static struct option l_opts[] = { { "verbose", no_argument, 0, 'v' },
                                  { "comput", no_argument, 0, 'c' },
                                  { "test-token", no_argument, 0, 't' },
                                  { "echo", no_argument, 0, 'h' },

                                  { 0, 0, 0, 0 } };

int main(int argc, char *argv[])
{
    char *input = calloc(1, sizeof(char));
    int c;
    int opt_idx = 0;
    while ((c = getopt_long(argc, argv, "vh:ct:", l_opts, &opt_idx)) != -1)
    {
        switch (c)
        {
        case 'v':
            logger(NULL, NULL);
            break;
        case 'c':
            fill_input_with_argv(argv, argc, optind, input);
            logger("%s", input);
            break;
        case 't':
            fill_input_with_argv(argv, argc, optind, input);
            logger("%s\n", input);
            test_lexer(input);
            logger("start token debug");
            break;
        case 'h':
            echo(argc - 1, argv + 1);
            break;

        case '?':
            exit(1);

        default:
            errx(1, "main: unkown option %c", c);
        }
    }

    free(input);
    return 0;
}
