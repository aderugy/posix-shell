#include <err.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast/nodes/node.h"
#include "builtins/commands.h"
#include "lexer/lexer.h"
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
            char *path = argv[1];
            stream = stream_from_file(path);
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
    register_commands();
    struct lexer *lexer = lexer_create(stream);
    struct ast_node *node = ast_create(lexer, AST_SIMPLE_COMMAND);

    ast_print(node);
    logger("\n");

    ast_eval(node, NULL);

    ast_free(node);

    lexer_free(lexer);
    unregister_commands();
    return 0;
}
