#include "naming.h"

#include <ctype.h>
#include <string.h>

#include "lexer/splitter.h"

// @RENAME

// @ANSWER
// rename the the way that pleases you, i can't guess it if you don't tell me

// Checks weither the name is XBD compatible
// 'In the shell command language, a word consisting solely of underscores,
// digits, and alphabetics from the portable character set.
// The first character of a name is not a digit.'
int convention_check(char *name, int len)
{
    if (isdigit(*name))
    {
        return 0;
    }

    int i = 0;
    while (name[i] && (isalnum(name[i]) || name[i] == '_'))
    {
        ++i;
    }

    return i == len;
}

// @RENAME

// @ANSWER
// rename the the way that pleases you, i can't guess it if you don't tell me

// Check weither the current character is UNQUOTED or in double quotes
// A dollar expansion will be invalid IF
// => the dollar is SINGLE_QUOTED or ESCAPED
int dollar_valid(int state)
{
    return state == SHARD_UNQUOTED || state == SHARD_DOUBLE_QUOTED;
}

// @RENAME

// @ANSWER
// rename the the way that pleases you, i can't guess it if you don't tell me

// Cf line 10.
int regular(int c)
{
    return isdigit(c) || strchr("@*#?$", c) || isalnum(c);
}

int XDB_valid(char *name)
{
    if (isdigit(*name))
    {
        return 0;
    }

    while (*name && (isalnum(*name) || *name == '_'))
    {
        ++name;
    }

    return *name == 0;
}
