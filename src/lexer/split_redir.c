#include "split_redir.h"

#include <string.h>

#include "mbtstr/str.h"
#include "splitter.h"
#include "utils/logger.h"

int is_redir(struct mbt_str *str)
{
    for (size_t i = 0; REDIRS[i]; i++)
    {
        char *sub = strstr(str->data, REDIRS[i]);
        if (!sub)
        {
            continue;
        }
        logger("testing : %s for sub %s\n", REDIRS[i], sub);
        if (strcmp(sub, REDIRS[i]) == 0)
        {
            return 1;
        }
    }

    return 0;
}

int split_redir(struct stream *stream, struct mbt_str *str, char c)
{
    mbt_str_pushc(str, c);
    if (is_redir(str)) // Case 2
    {
        logger("found token redir in splitter\n");
        stream_read(stream);
        return CONTINUE;
    }
    else // Case 3
    {
        logger("not found token redir in splitter ");
        for (size_t i = 0; i < str->size; i++)
        {
            logger(" %c", str->data[i]);
        }
        logger("\n");
        mbt_str_pop(str); // Not an operator -> We delimit
        return BREAK;
    }
}
