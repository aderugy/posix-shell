#include "dstream.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct dstream *dstream_init(FILE *in, FILE *in_state)
{
    if (!in || !in_state)
    {
        errx(EXIT_FAILURE, "no input stream provided");
    }

    struct dstream *dstream = calloc(1, sizeof(struct dstream));
    if (!dstream)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    setvbuf(in, NULL, _IOLBF, 0);
    setvbuf(in_state, NULL, _IOLBF, 0);

    dstream->in = in;
    dstream->in_state = in_state;

    dstream->next = fgetc(in);
    dstream->next_state = fgetc(in_state);

    return dstream;
}

struct dstream *dstream_from_str(char *str, char *str_state)
{
    FILE *in = fmemopen(str, strlen(str), "r");
    FILE *in_state = fmemopen(str_state, strlen(str_state), "r");
    return dstream_init(in, in_state);
}

void dstream_close(struct dstream *dstream)
{
    if (dstream)
    {
        if (dstream->in)
        {
            fclose(dstream->in);
        }

        if (dstream->in_state)
        {
            fclose(dstream->in_state);
        }
        free(dstream);
    }
}

/**
 * Reads one char from stream.
 * Returns the read char or EOF
 */
char dstream_read(struct dstream *stream)
{
    char c = stream->next;

    stream->next = fgetc(stream->in);
    stream->next_state = fgetc(stream->in_state);

    if (c == EOF)
    {
        stream->next = 0;
        stream->next_state = 0;
        return 0;
    }

    return c;
}

/**
 * Peeks at the next char in the stream, without modifying the cursor.
 * Returns the read char or EOF.
 */
char dstream_peek(struct dstream *stream)
{
    if (stream->next == EOF)
    {
        return 0;
    }
    return stream->next;
}

char dstream_peek_state(struct dstream *stream)
{
    return stream->next_state;
}
