#include "streams.h"

#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/logger.h"

static struct stream *stream_init(void)
{
    struct stream *stream = calloc(1, sizeof(struct stream));
    if (!stream)
    {
        logger("stream_init: out of memory");
        return NULL;
    }

    return stream;
}

struct stream *stream_from_file(const char *path)
{
    struct stream *stream = stream_init();
    if (!stream)
    {
        return NULL;
    }

    stream->in = fopen(path, "r");
    if (!stream->in)
    {
        logger("stream_from_file: failed to open stream\n");

        free(stream);
        return NULL;
    }

    return stream;
}

struct stream *stream_from_str(char *str)
{
    struct stream *stream = stream_init();
    if (!stream)
    {
        return NULL;
    }

    stream->in = fmemopen(str, strlen(str), "r");
    if (!stream->in)
    {
        logger("stream_from_str: failed to open stream\n");

        free(stream);
        return NULL;
    }

    return stream;
}

struct stream *stream_from_stream(FILE *in)
{
    if (!in)
    {
        return NULL;
    }

    struct stream *stream = stream_init();
    if (!stream)
    {
        return NULL;
    }

    stream->in = in;
    return stream;
}

void stream_close(struct stream *stream)
{
    if (stream)
    {
        fclose(stream->in);
        free(stream);
    }
}

/**
 * Reads one char from stream.
 * Returns the read char or EOF
 */
char stream_read(struct stream *stream)
{
    return fgetc(stream->in);
}

/**
* Peeks at the next char in the stream, without modifying the cursor.
* Returns the read char or EOF.
*/
char stream_peek(struct stream *stream)
{
    char c = fgetc(stream->in);
    if (c != EOF)
    {
        ungetc(c, stream->in);
    }

    return c;
}
