#include "streams.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/logger.h"

static struct stream *stream_init(FILE *in)
{
    if (!in)
    {
        errx(EXIT_FAILURE, "no input stream provided");
    }

    struct stream *stream = calloc(1, sizeof(struct stream));
    if (!stream)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    setvbuf(in, NULL, _IOLBF, 0);

    stream->in = in;
    return stream;
}

struct stream *stream_from_file(const char *path)
{
    return stream_init(fopen(path, "r"));
}

struct stream *stream_from_str(char *str)
{
    return stream_init(fmemopen(str, strlen(str), "r"));
}

struct stream *stream_from_stream(FILE *in)
{
    return stream_init(in);
}

void stream_close(struct stream *stream)
{
    if (stream)
    {
        if (stream->in)
        {
            fclose(stream->in);
        }

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
        stream->next = c;

        // logger("stream.c : ungetc done\n");
    }

    return c;
}
