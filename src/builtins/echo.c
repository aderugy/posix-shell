#include <err.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "builtins.h"
#include "commands.h"
#include "utils/xalloc.h"

static struct option l_opts[] = { { "e", no_argument, 0, 'e' },
                                  { "n", no_argument, 0, 'n' },
                                  { "E", no_argument, 0, 'E' },
                                  { 0, 0, 0, 0 } };

void print_echo(struct echo_options *opts, int argc)
{
    for (int j = 0; j < argc; j++)
    {
        size_t i = 0;
        while (*(opts->str[j] + i) != 0)
        {
            if (opts->interpret_backslash && *(opts->str[j] + i) == '\\')
            {
                i++;
                if (*(opts->str[j] + i) == 'n')
                {
                    putchar('\n');
                }
                else if (*(opts->str[j] + i) == 't')
                {
                    putchar('\t');
                }
                else
                {
                    putchar(*(opts->str[j] + i));
                }
            }
            else
            {
                fprintf(stdout, "%c", *(opts->str[j] + i));
            }
            i++;
        }
        if (j != argc - 1)
        {
            int k = j + 1;
            while (k < argc - 1 && *opts->str[k] == 0)
            {
                ++k;
            }
            int exist_next = (i > 0) && (*opts->str[k] != 0);
            if (exist_next)
            {
                putchar(' ');
            }
        }
    }
    if (!opts->not_newline)
        fprintf(stdout, "\n");
    fflush(stdout);
}

int echo(int argc, char *argv[])
{
    int c;
    int opt_idx = 0;

    struct echo_options *echo_opts = xmalloc(sizeof(struct echo_options));
    echo_opts->interpret_backslash = false;
    echo_opts->not_newline = false;
    echo_opts->str = NULL;

    optind = 1;
    while ((c = getopt_long(argc, argv, "enE", l_opts, &opt_idx)) != -1)
    {
        switch (c)
        {
        case 'e':
            echo_opts->interpret_backslash = true;
            echo_opts->not_interpret_backslash_default = false;
            break;
        case 'n':
            echo_opts->not_newline = true;
            break;
        case 'E':
            echo_opts->interpret_backslash = false;
            echo_opts->not_interpret_backslash_default = true;
            break;
        case '?':
        default:
            goto error;
        }
    }
    echo_opts->str = argv + optind;
    print_echo(echo_opts, argc - optind);

    free(echo_opts);

    return EXIT_SUCCESS;

error:
    free(echo_opts);
    return EXIT_FAILURE;
}
