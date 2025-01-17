#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ast/expansion/vars.h"
#include "ast/nodes/node.h"
#include "builtins/commands.h"
#include "lexer/lexer.h"
#include "mbtstr/str.h"
#include "streams/streams.h"
#include "utils/hash_map.h"
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
    int nb_args = 0;
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
        case 't':
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
            nb_args += init_args(argc, argv, ctx);
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

    init_dollar(ctx);
    init_hashtag(nb_args, ctx);

    register_commands();
    struct lexer *lexer = lexer_create(stream);

    struct ast_node *node;
    int return_value = 0;
    while ((node = ast_create(lexer, AST_INPUT)) && !return_value)
    {
        ast_print(node);
        return_value = ast_eval(node, NULL, ctx);

        ast_free(node);
    }

    if (!node && lexer->stream)
    {
        return_value = 2;
    }

    ast_eval_ctx_free(ctx);
    lexer_free(lexer);
    unregister_commands();
    return return_value;
}
