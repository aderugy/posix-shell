#ifndef DSTREAM_H
#define DSTREAM_H

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>

struct dstream
{
    FILE *in_state;
    FILE *in;
    char next_state;
    char next;
};

struct dstream *dstream_from_str(char *str, char *str_state);

void dstream_close(struct dstream *stream);

char dstream_read(struct dstream *stream);
char dstream_peek(struct dstream *stream);
char dstream_peek_state(struct dstream *stream);

#endif // !DSTREAM_H
