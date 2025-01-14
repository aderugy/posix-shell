#include "naming.h"

#include <ctype.h>

int convention_check(char *name, int len)
{
    if (isdigit(*name))
    {
        return 0;
    }

    int i = 0;
    while (i < len && (isalnum(name[i]) || name[i] == '_'))
    {
        ++i;
    }

    return i == len;
}
