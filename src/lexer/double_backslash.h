#ifndef DOUBLE_BACKSLASH_H
#define DOUBLE_BACKSLASH_H
#include <stddef.h>

#include "mbtstr/str.h"
#include "streams/streams.h"

int split_double_backslash(struct stream *stream, struct mbt_str *str, char c,
                           struct mbt_str *str_state);
#endif // !DOUBLE_BACKSLASH_H
