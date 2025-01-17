#include "commands.h"

int true_builtin(__attribute__((unused)) int argc,
                 __attribute__((unused)) char **argv)
{
    return 0;
}

int false_builtin(__attribute__((unused)) int argc,
                  __attribute__((unused)) char **argv)
{
    return 1;
}
