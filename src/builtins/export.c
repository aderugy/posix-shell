#include <err.h>
#include <getopt.h>

#include "commands.h"
static struct option l_opts[] = { { "p", no_argument, 0, 'p' },
                                  { 0, 0, 0, 0 } };
int export_builtin(int argc, char *argv[])
{
    int c;

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
    }

    return 0;
}
