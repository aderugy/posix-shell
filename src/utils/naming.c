#include "naming.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/splitter.h"

// checks weither the name respect the conventions
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

int dollar_valid(int state)
{
    return state == SHARD_UNQUOTED || state == SHARD_DOUBLE_QUOTED;
}

int regular(int c)
{
    return isdigit(c) || strchr("@*#?$", c) || isalnum(c);
}
/*
int is_bracket_end(struct dstream *dstream)
{
    int c = dstream_peek(dstream);
    return c == '}' && dollar_valid(dstream_peek_state(dstream));
}
*/
