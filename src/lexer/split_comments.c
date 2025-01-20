#include "split_comments.h"

#include "splitter.h"

int split_comments(struct stream *stream, char c)
{
    while ((c = stream_peek(stream)) != '\n' && c != 0 && c != -1)
    {
        stream_read(stream); // Discard every char until \n 0 and -1
                             // (check with numeric value)
    }

    return SPLIT_CONTINUE;
}
