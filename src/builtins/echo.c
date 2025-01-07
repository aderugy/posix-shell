#include <err.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "builtins.h"

static struct option l_opts[] = { { "e", no_argument, 0, 'e' },
                                  { 0, 0, 0, 0 } };

void print_echo(struct echo_options *opts, int argc)
{
    for (int j = 0; j < argc; j++)
    {
        size_t i = 0;
        while (*(opts->str[j] + i) != 0)
        {
            if (opts->interpret_backslash && *(opts->str[j]) == '\\')
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
                putchar(*(opts->str[j] + i));
            }
            i++;
        }
        putchar(' ');
    }
    if (!opts->not_newline)
        printf("\n");
}

int echo(int argc, char *argv[])
{
    int c;
    int opt_idx = 0;
    printf("echo %s\n", argv[1]);

    struct echo_options *opts = malloc(sizeof(struct echo_options));
    if (!opts)
    {
        perror("malloc");
        return 1;
    }
    opts->interpret_backslash = false;
    opts->not_newline = true;
    opts->str = NULL;
    while ((c = getopt_long(argc, argv, "e", l_opts, &opt_idx)) != -1)
    {
        switch (c)
        {
        case 'e':
            opts->interpret_backslash = true;
            opts->not_interpret_backslash_default = false;
            break;
        case '?':
            return 1;
        default:
            errx(1, "echo: unkown option %c", c);
        }
    }
    opts->str = argv + optind - 1;
    print_echo(opts, argc - optind);

    printf("\n");
    return 0;
}
