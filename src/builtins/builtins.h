#ifndef BUILTINS_H
#define BUILTINS_H

#include <stdbool.h>

struct echo_options
{
    bool not_newline;
    bool interpret_backslash;
    bool not_interpret_backslash_default;
    char **str;
};

#endif // !#ifndef BUILTINS_H
