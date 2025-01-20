#ifndef SPLITTER_H
#define SPLITTER_H

#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stdlib.h>

#include "mbtstr/str.h"
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

enum shard_quote_type
{
    SHARD_UNQUOTED = 0,
    SHARD_SINGLE_QUOTED,
    SHARD_DOUBLE_QUOTED,
    SHARD_BACKSLASH_QUOTED
};

enum shard_type
{
    SHARD_WORD = 0,
    SHARD_EXPANSION_VARIABLE,
    SHARD_EXPANSION_SUBSHELL,
    SHARD_EXPANSION_ARITH,
    SHARD_GLOBBING_STAR,
    SHARD_GLOBBING_QUESTIONMARK,
    SHARD_OPERATOR,
    SHARD_DELIMITER
};

enum shard_ctx_type
{
    SHARD_CONTEXT_NONE = 0,
    SHARD_CONTEXT_DOUBLE_QUOTES,
    SHARD_CONTEXT_EXPANSION,
};

struct shard
{
    char *state;
    char *data;

    bool can_chain;

    enum shard_quote_type quote_type;
    enum shard_type type;
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

// clang-tidy 'max 10 functions per file.h'
struct shard *splitter_peek(struct splitter_ctx *ctx);
struct shard *splitter_pop(struct splitter_ctx *ctx);

struct splitter_ctx *splitter_ctx_init(struct stream *stream);
void splitter_ctx_free(struct splitter_ctx *ctx);
void splitter_ctx_expect(struct splitter_ctx *ctx, int value);
enum shard_quote_type splitter_ctx_get_active_quote(struct splitter_ctx *ctx);

struct shard *shard_init(struct mbt_str *data, bool can_chain,
                         enum shard_type type,
                         enum shard_quote_type quote_type);
int shard_is_redir(struct mbt_str *str);
int shard_is_operator(struct mbt_str *str);
int shard_is_operator(struct mbt_str *str);
void shard_print(struct shard *shard);
void shard_free(struct shard *shard);

#endif // !SPLITTER_H
