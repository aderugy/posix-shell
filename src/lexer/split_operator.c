#include "split_operator.h"

#include <stdio.h>
#include <string.h>

#include "mbtstr/str.h"
#include "splitter.h"

int is_operator(struct mbt_str *str)
{
    for (size_t i = 0; OPERATORS[i]; i++)
    {
        if (strcmp(OPERATORS[i], str->data) == 0)
        {
            return 1;
        }
    }

    return 0;
}

int split_operator(struct stream *stream, struct mbt_str *str, char c)
{
    mbt_str_pushc(str, c);
    if (is_operator(str)) // Case 2
    {
        stream_read(stream);
        return CONTINUE;
    }
    else // Case 3
    {
        mbt_str_pop(str); // Not an operator -> We delimit
        return BREAK;
    }
}
