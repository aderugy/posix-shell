#include "splitter.h"

#include <ctype.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbtstr/str.h"

static const char *OPERATORS[] = { "&&" };
static const int OPERATORS_SIZE = 1;

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

static int is_operator(struct mbt_str *str)
{
    for (size_t i = 0; i < OPERATORS_SIZE; i++)
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

        // Case 4: Quoting
        if (strchr("\\\"\'", c) && quoted == SHARD_UNQUOTED)
        {
            mbt_str_pushc(str, c);
            stream_read(stream);

            switch (c)
            {
            case '\"':
                quoted = SHARD_DOUBLE_QUOTED;
                break;
            case '\'':
                quoted = SHARD_SINGLE_QUOTED;
                break;
            case '\\':
                errx(EXIT_FAILURE, "not implemented");
            default:
                errx(EXIT_FAILURE, "wtf");
            }

            continue;
        }

        // Case 5: Expansions
        if (strchr("$`", c))
        {
            errx(EXIT_FAILURE, "not implemented");
        }

        // Case 6: New operator
        bool is_op = false;
        for (size_t i = 0; i < OPERATORS_SIZE; i++)
        {
            if (OPERATORS[i][0] == c)
            {
                is_op = true;
            }
        }
        if (is_op) // Case 6: matched
        {
            mbt_str_pushc(str, c);
            stream_read(stream);
            continue;
        }

        // Case 7: delimiter
        if (isspace(c))
        {
            stream_read(stream);

            if (str->size)
            {
                break;
            }
            else
            {
                continue;
            }
        }

        // Case 8: existing word
        if (str->size)
        {
            mbt_str_pushc(str, c);
            stream_read(stream);
            continue;
        }

        // Case 9: comments
        if (c == '#')
        {
            while ((c = stream_peek(stream)) != '\n')
            {
                stream_read(stream); // Discard every char until \n
            }

            break;
        }

        // Case 10: new word: keep looping
        mbt_str_pushc(str, c);
        stream_read(stream);
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
