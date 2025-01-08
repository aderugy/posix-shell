#include "lexer.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "splitter.h"
#include "token.h"

static const struct keyword KEYWORDS[] = {
    { "if", TOKEN_IF },       { "fi", TOKEN_FI },     { "elif", TOKEN_ELIF },
    { "else", TOKEN_ELSE },   { "then", TOKEN_THEN }, { ";", TOKEN_SEMICOLON },
    { "\n", TOKEN_NEW_LINE }, { "'", TOKEN_QUOTE },   { NULL, TOKEN_EOF }
};

#define KEYWORDS_LEN (sizeof(KEYWORDS) / sizeof(KEYWORDS[0]) - 1)

struct lexer *lexer_new(struct stream *stream)
{
    struct lexer *res = calloc(1, sizeof(struct lexer));
    if (!res)
    {
        return NULL;
    }

    res->current_tok = lexer_all(stream);
    return res;
}

void lexer_free(struct lexer *lexer)
{
    struct token *curr = lexer->current_tok;
    while (curr)
    {
        struct token *next = curr->next;
        free(curr);
        curr = next;
    }
    free(lexer);
}

struct token *lexer_all(struct stream *stream)
{
    struct token *res = calloc(1, sizeof(struct token));
    res->type = TOKEN_ERROR;

    struct token *curr = res;

    struct shard *shard;
    while ((shard = splitter_next(stream)))
    {
        for (size_t i = 0; i < KEYWORDS_LEN; i++)
        {
            if (strcmp(shard->data, KEYWORDS[i].name) == 0)
            {
                curr->type = KEYWORDS[i].type;
                break;
            }
        }

        if (curr->type == TOKEN_ERROR)
        {
            curr->type = TOKEN_WORD;
            curr->value.c = strdup(shard->data);
        }

        shard = splitter_next(stream);
        struct token *next = calloc(1, sizeof(struct token));
        curr->next = next;
        curr = next;
    }
    curr->type = TOKEN_EOF;

    return res;
}

struct token lexer_peek(struct lexer *lexer)
{
    return *(lexer->current_tok);
}

struct token lexer_pop(struct lexer *lexer)
{
    struct token *res = lexer->current_tok;
    if (res)
    {
        lexer->current_tok = res->next;
    }
    return *res;
}
