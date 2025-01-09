#include "lexer.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "splitter.h"
#include "token.h"
#include "utils/logger.h"

static const struct keyword KEYWORDS[] = {
    { "if", TOKEN_IF },       { "fi", TOKEN_FI },     { "elif", TOKEN_ELIF },
    { "else", TOKEN_ELSE },   { "then", TOKEN_THEN }, { ";", TOKEN_SEMICOLON },
    { "\n", TOKEN_NEW_LINE }, { "'", TOKEN_QUOTE },   { NULL, TOKEN_EOF }
};

#define KEYWORDS_LEN (sizeof(KEYWORDS) / sizeof(KEYWORDS[0]) - 1)

struct lexer *lexer_create(struct stream *stream)
{
    struct lexer *res = calloc(1, sizeof(struct lexer));
    if (!res)
    {
        return NULL;
    }

    res->stream = stream;
    return res;
}

void lexer_free(struct lexer *lexer)
{
    stream_close(lexer->stream);
    free(lexer);
}

static struct token *lex(struct lexer *lexer)
{
    logger("lex\n");
    struct token *token = calloc(1, sizeof(struct token));
    if (!token)
    {
        errx(EXIT_FAILURE, "lex: memory error");
    }
    token->type = TOKEN_ERROR;

    struct shard *shard = splitter_next(lexer->stream);
    if (!shard)
    {
        free(token);
        return NULL;
    }

    for (size_t i = 0; i < KEYWORDS_LEN; i++)
    {
        if (strcmp(shard->data, KEYWORDS[i].name) == 0)
        {
            token->type = KEYWORDS[i].type;
            break;
        }
    }

    if (token->type == TOKEN_ERROR)
    {
        token->type = TOKEN_WORD;
        token->value.c = strdup(shard->data);
    }

    shard_free(shard);

    return token;
}

struct token *lexer_peek(struct lexer *lexer)
{
    if (!lexer->next)
    {
        lexer->next = lex(lexer);
    }

    return lexer->next;
}

struct token *lexer_pop(struct lexer *lexer)
{
    struct token *token = lexer->next ? lexer->next : lex(lexer);
    if (!token)
    {
        return NULL;
    }
    lexer->next = token->type != TOKEN_EOF ? lex(lexer) : NULL;

    return token;
}
