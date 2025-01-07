#include "lexer.h"

#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"

struct lexer *lexer_new(const char *input)
{
    struct lexer *res = calloc(1, sizeof(struct lexer));
    if (!res)
    {
        return NULL;
    }

    res->current_tok.type = TOKEN_ERROR;
    res->input = input;
    return res;
}

void lexer_free(struct lexer *lexer)
{
    free(lexer);
}

struct token lexer_next_token(struct lexer *lexer)
{
    struct token res;
    res.type = TOKEN_ERROR;

    const char *input = lexer->input;
    while ((input[lexer->pos]) == ' ')
    {
        (lexer->pos)++;
    }

    const char *current_char = input + lexer->pos;
    if (strncmp("if", current_char, 2) == 0)
    {
        res.type = TOKEN_IF;
        lexer->pos += 2;
    }
    else if (strncmp("then", current_char, 4) == 0)
    {
        res.type = TOKEN_THEN;
        lexer->pos += 4;
    }
    else if (strncmp("elif", current_char, 4) == 0)
    {
        res.type = TOKEN_ELIF;
        lexer->pos += 4;
    }
    else if (strncmp("else", current_char, 4) == 0)
    {
        res.type = TOKEN_ELSE;
        lexer->pos += 4;
    }
    else if (strncmp("fi", current_char, 2) == 0)
    {
        res.type = TOKEN_FI;
        lexer->pos += 2;
    }
    else if (current_char[0] == ';')
    {
        res.type = TOKEN_SEMICOLON;
        lexer->pos++;
    }
    else if (current_char[0] == '\n')
    {
        res.type = TOKEN_NEW_LINE;
        lexer->pos++;
    }
    else if (current_char[0] == '\'')
    {
        res.type = TOKEN_QUOTE;
        lexer->pos++;
    }
    else if (current_char[0] == 0)
    {
        res.type = TOKEN_EOF;
        lexer->pos++;
    }
    else
    {
        res.type = TOKEN_WORD;

        size_t len = 0;
        while (!(current_char[len] && strchr("; \'\n", current_char[len])))
        {
            len++;
        }

        res.value.c = calloc(len + 1, sizeof(char));
        memcpy(res.value.c, current_char, len);

        lexer->pos += len;
    }

    return res;
}

struct token lexer_peek(struct lexer *lexer)
{
    int pos = lexer->pos;

    struct token res = lexer_next_token(lexer);
    lexer->pos = pos;

    return res;
}

struct token lexer_pop(struct lexer *lexer)
{
    lexer->current_tok = lexer_next_token(lexer);
    return lexer->current_tok;
}
