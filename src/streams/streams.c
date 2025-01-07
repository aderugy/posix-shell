#include "streams.h"

#include <stdlib.h>
#include <string.h>
#include <utils/logger.h>

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
 * Returns -1 on EOF
 */
char stream_read(struct stream *stream)
{
    char buffer[1];
    if (!fread(buffer, 1, 1, stream->in))
    {
        logger("EOF\n");
        return -1;
    }

    return buffer[0];
}
