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

static const char *OPERATORS[] = { ";", "&&", "&", "|", "||", "!", NULL };
static const char *REDIRS[] = { ">>", ">&", "<&", "<>", ">", "<", NULL };
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
static int is_redir(struct mbt_str *str)
{
    for (size_t i = 0; REDIRS[i]; i++)
    {
        char *sub = strstr(str->data, REDIRS[i]);
        if (!sub)
        {
            continue;
        }
        logger("testing : %s for sub %s\n", REDIRS[i], sub);
        if (strcmp(sub, REDIRS[i]) == 0)
        {
            return 1;
        }
    }

    return 0;
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
    struct mbt_str *str_state = mbt_str_init(64);

    char c;
    // logger("splitter.c : will stream peek\n");
    while ((c = stream_peek(stream)) != EOF)
    {
        // Case 1: EOF handled by exiting the loop

        // Case 2-3: Operators
        if (is_operator(str))
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
        if (is_redir(str))
        {
            mbt_str_pushc(str, c);
            if (is_redir(str)) // Case 2
            {
                logger("found token redir in splitter\n");
                stream_read(stream);
                continue;
            }
            else // Case 3
            {
                logger("not found token redir in splitter ");
                for (size_t i = 0; i < str->size; i++)
                {
                    logger(" %c", str->data[i]);
                }
                logger("\n");
                mbt_str_pop(str); // Not an operator -> We delimit
                break;
            }
        }

        if (c == '\\')
        {
            stream_read(stream);
            char next = stream_read(stream);
            if (next == EOF)
            {
                break;
            }
            logger("next : %c", c);

            mbt_str_fill(str, str_state, SHARD_UNQUOTED);
            mbt_str_pushc(str, next);
            mbt_str_pushc(str_state, SHARD_BACKSLASH_QUOTED);

            // logger("splitter.c : will stream peek\n");
            continue;
        }

        // Case 4: Quoting
        int ret_val;
        if (strchr("\\\"\'", c))
        {
            ret_val = handle_quoting(stream, str, str_state, c);
            if (ret_val == BREAK)
            {
                break;
            }
            else if (ret_val == CONTINUE)
            {
                // logger("splitter.c : will stream peek\n");
                continue;
            }
        }

        // Case 5 to 11 included
        ret_val = handle_5_to_11(stream, str, c);
        if (ret_val == BREAK)
        {
            break;
        }
        else if (ret_val == CONTINUE)
        {
            // logger("splitter.c : will stream peek\n");
            continue;
        }

        // logger("splitter.c : will stream peek\n");
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
        logger("splitter : %c\n", c);

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
