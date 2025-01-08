#include "lexer.h"

#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "splitter.h"
#include "token.h"

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
    struct token *curr = res;
    res->type = TOKEN_ERROR;
    struct shard *shard = splitter_next(stream);
    while (shard)
    {
        char *input = shard->data;
        while (isspace(*input))
        {
            input++;
        }
        const char *current_char = input;
        if (strncmp("if", current_char, 2) == 0)
        {
            curr->type = TOKEN_IF;
        }
        else if (strncmp("then", current_char, 4) == 0)
        {
            curr->type = TOKEN_THEN;
        }
        else if (strncmp("elif", current_char, 4) == 0)
        {
            curr->type = TOKEN_ELIF;
        }
        else if (strncmp("else", current_char, 4) == 0)
        {
            curr->type = TOKEN_ELSE;
        }
        else if (strncmp("fi", current_char, 2) == 0)
        {
            curr->type = TOKEN_FI;
        }
        else if (current_char[0] == ';')
        {
            curr->type = TOKEN_SEMICOLON;
        }
        else if (current_char[0] == '\n')
        {
            curr->type = TOKEN_NEW_LINE;
        }
        else if (current_char[0] == '\'')
        {
            curr->type = TOKEN_QUOTE;
        }
        else if (current_char[0] == 0)
        {
            curr->type = TOKEN_EOF;
        }
        else
        {
            curr->type = TOKEN_WORD;

            size_t len = 0;
            while (current_char[len] && !strchr("; \'\n", current_char[len]))
            {
                len++;
            }

            curr->value.c = calloc(len + 1, sizeof(char));
            memcpy(curr->value.c, current_char, len);
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
