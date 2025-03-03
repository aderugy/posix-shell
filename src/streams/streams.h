#ifndef STREAMS_H
#define STREAMS_H

#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stdio.h>

struct stream
{
    FILE *in;
    char next;
    bool tty;
};

struct stream *stream_from_file(const char *path);
struct stream *stream_from_str(char *str);
struct stream *stream_from_stream(FILE *in);

void stream_close(struct stream *stream);

char stream_read(struct stream *stream);
char stream_peek(struct stream *stream);

void stream_empty(struct stream *stream);

#endif // !STREAMS_H
