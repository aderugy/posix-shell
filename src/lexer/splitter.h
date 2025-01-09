#ifndef SPLITTER_H
#define SPLITTER_H

#include "streams/streams.h"

#define SHARD_UNQUOTED 0
#define SHARD_SINGLE_QUOTED 1
#define SHARD_DOUBLE_QUOTED 2
#define SHARD_BACKSLASH_QUOTED 3

#define SPLIT_ERROR 2

struct shard
{
    char quoted;
    char *data;
};

struct shard *splitter_next(struct stream *stream);

void shard_free(struct shard *shard);

#endif // !SPLITTER_H
