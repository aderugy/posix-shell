#include "double_backslash.h"
#include "split_comments.h"
#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbtstr/str.h"
#include "split_operator.h"
#include "split_redir.h"
#include "splitter.h"
#include "utils/logger.h"

static struct shard *shard_init(char *data, char *state)
{
    struct shard *shard = calloc(1, sizeof(struct shard));
    if (!shard)
    {
        errx(EXIT_FAILURE, "shard_init: memory error");
    }

    shard->data = data;
    shard->state = state;
    return shard;
}

void shard_free(struct shard *shard)
{
    free(shard->data);
    free(shard->state);
    free(shard);
}

struct shard *splitter_next(struct stream *stream)
{
    struct mbt_str *str = mbt_str_init(64);
    struct mbt_str *str_state = mbt_str_init(64);

    char c;
    // logger("splitter.c : will stream peek\n");
    while ((c = stream_peek(stream)) != EOF)
    {
        // Case 1: EOF handled by exiting the loop
        // Case 2-3: Operators
        int ret_val = -1;
        if (is_operator(str))
        {
            ret_val = split_operator(stream, str, c);
        }
        if (is_redir(str))
        {
            ret_val = split_redir(stream, str, c);
        }

        if (c == '\\' || strchr("\\\"\'", c))
        {
            ret_val = split_double_backslash(stream, str, c, str_state);
        }

        // Case 5 to 11 included
        if (ret_val == -1)
        {
            ret_val = handle_5_to_11(stream, str, c);
        }
        if (ret_val == BREAK)
        {
            break;
        }
        else if (ret_val == CONTINUE)
        {
            continue;
        }
    }

    char *data = NULL;
    char *state = NULL;
    if (str->size)
    {
        mbt_str_fill(str, str_state, SHARD_UNQUOTED);
        data = strdup(str->data);
        state = strdup(str_state->data);
    }
    mbt_str_free(str);
    mbt_str_free(str_state);

    if (data)
    {
        return shard_init(data, state);
    }

    return NULL;
}

int handle_quoting(struct stream *stream, struct mbt_str *str,
                   struct mbt_str *str_state, char c)
{
    // Case 4: Quoting
    mbt_str_fill(str, str_state, SHARD_UNQUOTED);
    char quote = c;
    stream_read(stream);
    while ((c = stream_read(stream)) != EOF && c != quote)
    {
        if (c == '\\' && quote != '\'')
        {
            c = stream_read(stream);

            if (c == EOF)
            {
                break;
            }
        }
        mbt_str_pushc(str, c);
    }

    switch (c)
    {
    case '\"':
        mbt_str_fill(str, str_state, SHARD_DOUBLE_QUOTED);
        return CONTINUE;
    case '\'':
        mbt_str_fill(str, str_state, SHARD_SINGLE_QUOTED);
        return CONTINUE;
    case '\\':
        stream_read(stream);
        c = stream_peek(stream);

        if (c == EOF)
        {
            break;
        }

        stream_read(stream);
        if (c != '\n')
        {
            mbt_str_pushc(str, c);
            mbt_str_pushc(str, SHARD_BACKSLASH_QUOTED);
        }
        break;
    case EOF:
        errx(SPLIT_ERROR, "bro unmatched quote");
    default:
        errx(EXIT_FAILURE, "wtf");
    }

    // return BREAK;
    return CONTINUE;
}

int handle_5_to_11(struct stream *stream, struct mbt_str *str, char c)
{
    // Case 5: Expansions
    // Case 6: New operator
    bool is_op = false;
    for (size_t i = 0; OPERATORS[i]; i++)
    {
        if (OPERATORS[i][0] == c)
        {
            is_op = true;
        }
    }
    if (is_op) // Case 6: matched
    {
        if (str->size)
        {
            return BREAK;
        }

        mbt_str_pushc(str, c);
        stream_read(stream);
        return CONTINUE;
    }
    bool is_redir = false;
    for (size_t i = 0; REDIRS[i]; i++)
    {
        if (REDIRS[i][0] == c)
        {
            is_redir = true;
        }
    }
    if (is_redir) // Case 6: matched
    {
        mbt_str_pushc(str, c);
        stream_read(stream);
        return CONTINUE;
    }

    // Case 7: Newlines
    if (c == '\n')
    {
        if (!str->size)
        {
            mbt_str_pushc(str, c);
            stream_read(stream);
        }
        return BREAK;
    }

    // Case 8: delimiter
    if (isspace(c))
    {
        stream_read(stream);

        if (str->size)
        {
            return BREAK;
        }
        else
        {
            return CONTINUE;
        }
    }

    // Case 9: existing word
    if (str->size)
    {
        mbt_str_pushc(str, c);
        stream_read(stream);
        return CONTINUE;
    }

    // Case 10: comments
    if (c == '#')
    {
        return split_comments(stream, c);
    }

    // Case 11: new word: keep looping
    mbt_str_pushc(str, c);
    stream_read(stream);

    return DO_NOTHING;
}
