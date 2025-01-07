#ifndef BUILTINS_H
#define BUILTINS_H

#include <stdbool.h>

int echo(int argc, char **argv);
struct echo_options
{
    bool not_newline;
    bool interpret_backslash;
    bool not_interpret_backslash_default;
    char **str;
};

int true_builtin(void);
int false_builtin(void);

#endif // !#ifndef BUILTINS_H
