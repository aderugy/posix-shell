#ifndef SHARD_H
#define SHARD_H

#include <stdbool.h>

#include "mbtstr/str.h"

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

struct shard
{
    char *state;
    char *data;

    bool can_chain;

    enum shard_quote_type quote_type;
    enum shard_type type;
};

struct shard *shard_init(struct mbt_str *data, bool can_chain,
                         enum shard_type type,
                         enum shard_quote_type quote_type);
int shard_is_redir(struct mbt_str *str);
int shard_is_operator(struct mbt_str *str);
int shard_is_operator(struct mbt_str *str);
void shard_print(struct shard *shard);
void shard_free(struct shard *shard);

#endif // !SHARD_H
