#ifndef SPLITTER_H
#define SPLITTER_H

#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stdlib.h>

#include "mbtstr/str.h"
#include "shard.h"
#include "streams/streams.h"
#include "utils/stack.h"

#define SHARD_ERROR -1

#define IS_REDIR_FALSE 0
#define IS_REDIR_PARTIAL 1
#define IS_REDIR_MATCH 2

#define NOT_EMPTY(str) str->size > 0
#define DELIMITER(str) shard_init(str, false, SHARD_DELIMITER, SHARD_UNQUOTED);

#define DEFINE_OPERATORS                                                       \
    static const char *OPERATORS[] = { ";", "&&", "&", "|", "||", NULL }

#define DEFINE_REDIRS                                                          \
    static const char *REDIRS[] = { ">>", ">&", "<&", "<>", ">|",              \
                                    "|>", ">",  "<",  NULL }

#define DEFINE_VARIABLES                                                       \
    static const char *VARIABLES[] = { "@", "*", "#", "?", "-", "$", "!", NULL }

enum shard_ctx_type
{
    SHARD_CONTEXT_NONE = 0,
    SHARD_CONTEXT_DOUBLE_QUOTES,
    SHARD_CONTEXT_EXPANSION,
};

struct splitter_ctx_exp
{
    enum shard_ctx_type value;
};

struct splitter_ctx
{
    bool err;
    struct stack *expect;

    struct stream *stream;
    struct shard *cache;
};

struct shard *splitter_peek(struct splitter_ctx *ctx);
struct shard *splitter_pop(struct splitter_ctx *ctx);

struct splitter_ctx *splitter_ctx_init(struct stream *stream);
void splitter_ctx_free(struct splitter_ctx *ctx);
void splitter_ctx_expect(struct splitter_ctx *ctx, int value);
enum shard_quote_type splitter_ctx_get_active_quote(struct splitter_ctx *ctx);

#endif // !SPLITTER_H
