#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <string.h>

#include "splitter.h"
#include "utils/logger.h"
#include "utils/xalloc.h"

DEFINE_OPERATORS;
DEFINE_REDIRS;

void shard_print(struct shard *shard)
{
    if (!shard)
    {
        logger("(nil)\n");
        return;
    }

    char *type = NULL;
    switch (shard->type)
    {
    case SHARD_EXPANSION_VARIABLE:
        type = "VAR      ";
        break;
    case SHARD_EXPANSION_SUBSHELL:
        type = "SUBSHELL ";
        break;
    case SHARD_EXPANSION_ARITH:
        type = "ARITH    ";
        break;
    case SHARD_GLOBBING_STAR:
        type = "GLOB STAR";
        break;
    case SHARD_GLOBBING_QUESTIONMARK:
        type = "GLOB QM  ";
        break;
    case SHARD_DELIMITER:
        type = "DELIMITER";
        break;
    default:
        type = "WORD     ";
        break;
    }

    logger("%s%s ", shard->can_chain ? "*" : " ", type);
    if (shard->quote_type)
    {
        logger("%c", shard->quote_type == SHARD_SINGLE_QUOTED ? '\'' : '"');
    }
    logger("%s", shard->data);
    if (shard->quote_type)
    {
        logger("%c", shard->quote_type == SHARD_SINGLE_QUOTED ? '\'' : '"');
    }

    logger("\n");
}

struct shard *shard_init(struct mbt_str *data, bool can_chain,
                         enum shard_type type, enum shard_quote_type quote_type)
{
    struct shard *shard = xcalloc(1, sizeof(struct shard));
    shard->data = strdup(data->data);
    shard->can_chain = can_chain;
    shard->type = type;
    shard->quote_type = quote_type;

    mbt_str_free(data);
    return shard;
}

void shard_free(struct shard *shard)
{
    if (shard)
    {
        free(shard->data);
        free(shard);
    }
}

int shard_is_redir(struct mbt_str *str)
{
    int status = IS_REDIR_FALSE;

    if (!str->size)
    {
        return status;
    }

    char *redir = str->data;
    if (isdigit(*redir))
    {
        redir++;
    }

    if (!*redir)
    {
        return status;
    }

    for (size_t i = 0; REDIRS[i]; i++)
    {
        int sstatus = IS_REDIR_FALSE;

        for (size_t j = 0;; j++)
        {
            char c1 = REDIRS[i][j];
            char c2 = redir[j];
            if (!c1 && !c2)
            {
                sstatus = IS_REDIR_MATCH;
                break;
            }
            else if (c1 && !c2)
            {
                sstatus = IS_REDIR_PARTIAL;
            }
            else if (c1 && c2)
            {
                if (c1 != c2)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }

        if (sstatus > status)
        {
            status = sstatus;
        }
    }

    return status;
}

int shard_is_operator(struct mbt_str *str)
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
