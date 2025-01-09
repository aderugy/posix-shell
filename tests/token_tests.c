#include <err.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
#include "lexer/splitter.h"
#include "lexer/token.h"
#include "streams/streams.h"
#include "utils/logger.h"

static struct option l_opts[] = { { "verbose", no_argument, 0, 'v' },
                                  { "comput", required_argument, 0, 'c' },
                                  { "test-token", no_argument, 0, 't' },

                                  { 0, 0, 0, 0 } };

/*int test_lexer(struct stream *stream)
{
    char *tab[] = { [TOKEN_IF] = "IF",
                    [TOKEN_THEN] = "THEN",
                    [TOKEN_ELIF] = "ELIF",
                    [TOKEN_ELSE] = "ELSE",
                    [TOKEN_FI] = "FI",
                    [TOKEN_SEMICOLON] = ";",
                    [TOKEN_NEW_LINE] = "NEW_LINE",
                    [TOKEN_QUOTE] = "QUOTE" };
    struct lexer *lexer = lexer_create(stream);
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
}*/

int main(int argc, char *argv[])
{
    int c;
    int opt_idx = 0;
    struct stream *stream = NULL;
    while ((c = getopt_long(argc, argv, "vc:t", l_opts, &opt_idx)) != -1)
    {
        switch (c)
        {
        case 'v':
            logger(NULL, NULL);
            logger("--verbose is activated\n");
            break;
        case 'c':
            stream = stream_from_str(optarg);
            break;
        case '?':
            exit(1);

        default:
            errx(1, "main: unkown option %c", c);
        }
    }
    struct lexer *lexer = lexer_create(stream);
    return 0;
}
