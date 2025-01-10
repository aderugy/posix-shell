#define _POSIX_C_SOURCE 200809L

#include "splitter.h"

#include <ctype.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbtstr/str.h"
#include "utils/logger.h"

static const char *OPERATORS[] = { ";",  "&&", "&",  "|",  "||", ">", "<",
                                   ">>", ">&", "<&", ">,", "<>", NULL };

static struct shard *shard_init(char quoted, char *data)
{
    struct shard *shard = calloc(1, sizeof(struct shard));
    if (!shard)
    {
        errx(EXIT_FAILURE, "shard_init: memory error");
    }

    shard->quoted = quoted;
    shard->data = data;
    return shard;
}

void shard_free(struct shard *shard)
{
    free(shard->data);
    free(shard);
}

static int is_operator(struct mbt_str *str)
{
    for (size_t i = 0; OPERATORS[i]; i++)
    {
        if (strcmp(OPERATORS[i], str->data) == 0)
        {
            return 1;
        }
    }

    return 0;
}

struct shard *splitter_next(struct stream *stream)
{
    struct mbt_str *str = mbt_str_init(64);

    char c;
    char quoted = SHARD_UNQUOTED;
    while ((c = stream_peek(stream)) != EOF)
    {
        // Case 1: EOF handled by exiting the loop

        // Case 2-3: Operators
        if (is_operator(str) && quoted == SHARD_UNQUOTED)
        {
            mbt_str_pushc(str, c);
            if (is_operator(str)) // Case 2
            {
                stream_read(stream);
                continue;
            }
            else // Case 3
            {
                mbt_str_pop(str); // Not an operator -> We delimit
                break;
            }
        }

        if (c == '\\' && quoted == SHARD_UNQUOTED)
        {
            stream_read(stream);
            char next = stream_read(stream);
            if (next == EOF)
            {
                break;
            }

            mbt_str_pushc(str, next);
            quoted = SHARD_BACKSLASH_QUOTED;
            continue;
        }

        // Case 4: Quoting
        if (strchr("\\\"\'", c) && quoted == SHARD_UNQUOTED)
        {
            if (handle_quoting(stream, str, &quoted, c) == BREAK)
            {
                break;
            }
        }

        // Case 5 to 11 included
        int ret_val = handle_5_to_11(stream, str, c);
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
    if (str->size)
    {
        data = strdup(str->data);
    }
    mbt_str_free(str);

    if (data)
    {
        return shard_init(quoted, data);
    }

    return NULL;
}

int handle_quoting(struct stream *stream, struct mbt_str *str, char *quoted,
                   char c)
{
    if (str->size)
    {
        return BREAK;
    }

    char quote = c;
    stream_read(stream);
    while ((c = stream_read(stream)) != EOF && c != quote)
    {
        if (c == '\\')
        {
            c = stream_read(stream);

            if (c == EOF)
            {
                return BREAK;
            }

            if (c != quote)
            {
                mbt_str_pushc(str, '\\');
            }
        }

        mbt_str_pushc(str, c);
    }

    switch (c)
    {
    case '\"':
        *quoted = SHARD_DOUBLE_QUOTED;
        break;
    case '\'':
        *quoted = SHARD_SINGLE_QUOTED;
        break;
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
        }
        break;
    case EOF:
        errx(SPLIT_ERROR, "unmatched quote");
    default:
        errx(EXIT_FAILURE, "wtf");
    }

    return BREAK;
}

int handle_5_to_11(struct stream *stream, struct mbt_str *str, char c)
{
    // Case 5: Expansions
    if (strchr("$`", c))
    {
        errx(EXIT_FAILURE, "not implemented");
    }
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
        while ((c = stream_peek(stream)) != '\n' && c != 0 && c != -1)
        {
            stream_read(stream); // Discard every char until \n 0 and -1
                                 // (check with numeric value)
        }
        logger("\n");

        return CONTINUE;
    }

    // Case 11: new word: keep looping
    mbt_str_pushc(str, c);
    stream_read(stream);

    return DO_NOTHING;
}
