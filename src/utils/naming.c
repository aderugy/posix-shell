#include "naming.h"

#include <ctype.h>
#include <string.h>

#include "lexer/splitter.h"

// @RENAME
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

// @RENAME
int dollar_valid(int state)
{
    return state == SHARD_UNQUOTED || state == SHARD_DOUBLE_QUOTED;
}

// @RENAME
int regular(int c)
{
    return isdigit(c) || strchr("@*#?$", c) || isalnum(c);
}
