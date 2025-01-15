#include "expansion.h"

#include <ctype.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>

char *expand_word(__attribute__((unused)) struct token *token)
{
    // struct mbt_str *str = mbt_str_init(10);
    char *data = token->value.c;

    while (*data != EOF)
    {
        while (strchr("$\0", *data) == NULL)
        {
            ++data;
        }

        if (*data != EOF)
        {
            ++data;
            if (*data == '{')
            {
                errx(EXIT_FAILURE, "expansion_word: not implemented");
            }
        }
    }

    return NULL;
}
