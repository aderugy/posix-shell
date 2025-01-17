#include "double_backslash.h"

#include <string.h>

#include "splitter.h"
#include "utils/logger.h"

int split_double_backslash(struct stream *stream, struct mbt_str *str, char c,
                           struct mbt_str *str_state)
{
    if (c == '\\')
    {
        stream_read(stream);

        char next = stream_read(stream);

        if (next == EOF)
        {
            return BREAK;
        }
        logger("next : %c", c);

        mbt_str_fill(str, str_state, SHARD_UNQUOTED);
        mbt_str_pushc(str, next);
        mbt_str_pushc(str_state, SHARD_BACKSLASH_QUOTED);

        return CONTINUE;
    }

    int double_quoting_context;

    // Case 4: Quoting
    if (strchr("\\\"\'", c))
    {
        double_quoting_context = handle_quoting(stream, str, str_state, c);
        if (double_quoting_context == BREAK)
        {
            return CONTINUE;
        }
        else if (double_quoting_context == CONTINUE)
        {
            return CONTINUE;
        }
    }
    return -1;
}
