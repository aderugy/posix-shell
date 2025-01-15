#ifndef SPLITTER_H
#define SPLITTER_H

#include "mbtstr/str.h"
#include "streams/streams.h"

#define SHARD_UNQUOTED 1
#define SHARD_SINGLE_QUOTED 2
#define SHARD_DOUBLE_QUOTED 3
#define SHARD_BACKSLASH_QUOTED 4

#define CONTINUE 0
#define BREAK 1
#define SPLIT_ERROR 2
#define DO_NOTHING 3

struct shard
{
    char *state;
    char *data;
};

struct shard *splitter_next(struct stream *stream);
int handle_5_to_11(struct stream *stream, struct mbt_str *str, char c);
int handle_quoting(struct stream *stream, struct mbt_str *str,
                   struct mbt_str *str_state, char c);

void shard_free(struct shard *shard);

#endif // !SPLITTER_H
