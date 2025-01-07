#include "lexer.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"

struct lexer *lexer_new(const char *input)
{
    struct lexer *res = calloc(1, sizeof(struct lexer));
    res->input = input;
    res->pos = 0;
    res->current_tok.type = TOKEN_ERROR;
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
    while ((lexer->input)[lexer->pos] == ' ')
        (lexer->pos)++;
    const char *current_char = (lexer->input) + (lexer->pos);
    if (strncmp("if", current_char, 2) == 0)
    {
        lexer->pos += 2;
        res.type = TOKEN_IF;
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
        while (*(current_char + len) != '\n' && *(current_char + len) != 0
               && *(current_char + len) != ';' && *(current_char + len) != '\''
               && *(current_char + len) != ' ')
            len++;
        res.value.c = malloc(len + 1);
        for (size_t i = 0; i < len; i++)
        {
            (res.value.c)[i] = *(current_char + i);
        }
        (res.value.c)[len] = 0;
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

// FOR JULES SUPRR

int test_lexer(char *str)
{
    char *tab[] = { [TOKEN_IF] = "IF",
                    [TOKEN_THEN] = "THEN",
                    [TOKEN_ELIF] = "ELIF",
                    [TOKEN_ELSE] = "ELSE",
                    [TOKEN_FI] = "FI",
                    [TOKEN_SEMICOLON] = ";",
                    [TOKEN_NEW_LINE] = "NEW_LINE",
                    [TOKEN_QUOTE] = "QUOTE" };
    struct lexer *lexer = lexer_new(str);
    struct token token = lexer_pop(lexer);

    while (token.type != TOKEN_EOF && token.type != TOKEN_ERROR)
    {
        if (token.type == TOKEN_WORD)
            printf("word: %s\n", token.value.c);
        else
            printf("%s\n", tab[token.type]);

        token = lexer_pop(lexer);
    }

    if (token.type == TOKEN_EOF)
        printf("EOF\n");

    lexer_free(lexer);

    return 0;
}
