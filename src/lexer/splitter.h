#ifndef SPLITTER_H
#define SPLITTER_H

#include <stdbool.h>

#include "streams/streams.h"
#include "utils/stack.h"

#define SHARD_ERROR -1

#define SHARD_UNQUOTED 0
#define SHARD_SINGLE_QUOTED 1
#define SHARD_DOUBLE_QUOTED 2
#define SHARD_BACKSLASH_QUOTED 3

#define SHARD_EXPANSION_VARIABLE 4
#define SHARD_EXPANSION_SUBSHELL 5
#define SHARD_EXPANSION_ARITH 6

#define SHARD_CONTEXT_DOUBLE_QUOTES 1
#define SHARD_CONTEXT_EXPANSION 2

struct shard
{
    char *state;
    char *data;

    bool can_chain;
    int shard_type;
};

struct splitter_ctx_exp
{
    int value;
};

struct splitter_ctx
{
    struct stack *expect;
};

struct shard *splitter_next(struct stream *stream, struct splitter_ctx *ctx);

struct splitter_ctx *splitter_ctx_init(void);
void splitter_ctx_expect(struct splitter_ctx *ctx, int value);

void shard_print(struct shard *shard);
void shard_free(struct shard *shard);

#endif // !SPLITTER_H
