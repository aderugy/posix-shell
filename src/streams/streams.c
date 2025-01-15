#include "streams.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils///logger.h"

static struct stream *stream_init(void)
{
    // logger("stream_init: init\n");
    struct stream *stream = calloc(1, sizeof(struct stream));
    if (!stream)
    {
        // logger("stream_init: out of memory");
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
    // logger("stream_from_file: %s\n", path);
    if (!stream->in)
    {
        // logger("stream_from_file: failed to open stream\n");

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
    // logger("stream_from_str: %s\n", str);
    if (!stream->in)
    {
        // logger("stream_from_str: failed to open stream\n");

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
        // logger("stream_from_stream: failed to init stream\n");
        return NULL;
    }
    // logger("stream_from_stream: %p\n", in);

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
    if (stream->next)
    {
        char c = stream->next;
        stream->next = 0;
        return c;
    }
    return fgetc(stream->in);
}

/**
 * Peeks at the next char in the stream, without modifying the cursor.
 * Returns the read char or EOF.
 */
char stream_peek(struct stream *stream)
{
    if (stream->next)
    {
        return stream->next;
    }
    char c = fgetc(stream->in);
    // logger("stream.c : will fgetc\n");
    if (c != EOF)
    {
        logger("stream.c : fgetc done\n");
        stream->next = c;

        // logger("stream.c : ungetc done\n");
    }

    return c;
}
