#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ast/expansion/vars.h"
#include "ast/nodes/eval_ctx.h"
#include "ast/nodes/node.h"
#include "builtins/commands.h"
#include "lexer/lexer.h"
#include "lexer/splitter.h"
#include "streams/streams.h"
#include "utils/logger.h"

static struct option l_opts[] = { { "verbose", no_argument, 0, 'v' },
                                  { "comput", required_argument, 0, 'c' },
                                  { "test-token", no_argument, 0, 't' },

                                  { 0, 0, 0, 0 } };

int main(int argc, char *argv[])
{
    int c;
    int opt_idx = 0;
    struct stream *stream = NULL;
    struct ast_eval_ctx *ctx = ctx_init();
    while ((c = getopt_long(argc, argv, "vc:t", l_opts, &opt_idx)) != -1)
    {
        switch (c)
        {
        case 'v':
            logger(NULL, NULL);
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

    if (!stream)
    {
        if (argc > 1)
        {
            char *path = argv[optind];
            stream = stream_from_file(path);
            init_args(argc, argv, ctx);
        }
        else
        {
            stream = stream_from_stream(stdin);
        }
    }

    if (!stream)
    {
        errx(1, "stream error");
    }

    struct lexer *lexer = lexer_create(stream);
    struct token *token;
    while ((token = lexer_pop(lexer)))
    {
        token_print(token);
        token_free(token);
    }

    lexer_free(lexer);
    ast_eval_ctx_free(ctx);
}
