#ifndef SPLIT_COMMENTS_H
#define SPLIT_COMMENTS_H
#include <stddef.h>

#include "mbtstr/str.h"
#include "streams/streams.h"

int split_comments(struct stream *stream, char c);
#endif // !SPLIT_COMMENTS_H
