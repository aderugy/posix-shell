#include <stdio.h>

#include "lexer/lexer.h"
#include "lexer/token.h"

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
