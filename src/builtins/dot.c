#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast/expansion/vars.h"
#include "ast/nodes/eval_ctx.h"
#include "ast/nodes/node.h"
#include "builtins/commands.h"
#include "lexer/lexer.h"
#include "streams/streams.h"
#include "utils/logger.h"

int dot(__attribute__((unused)) int argc, __attribute__((unused)) char **argv,
        __attribute__((unused)) struct ast_eval_ctx *ast_eval_ctx)
{
    if (argc != 2)
    {
        errx(2, "dot: param error");
    }
    logger("argv[1] = %s\n", argv[1]);
    struct stream *stream = NULL;
    stream = stream_from_file(argv[1]);
    logger("pass stream_from_file\n");
    if (!stream)
    {
        warnx("dot: stream error");
        return 2;
    }
    struct lexer *lexer = lexer_create(stream);
    struct ast_node *node;
    int return_value = 0;

    /*
     * Process input line by line (AST_INPUT after AST_INPUT)
     */
    while ((node = ast_create(lexer, AST_INPUT)) && !return_value)
    {
        ast_print(node);
        return_value = ast_eval(node, NULL, ast_eval_ctx);

        ast_free(node);
    }

    if (!node && lexer->stream)
    {
        warnx("Syntax error");
        return_value = 2;
    }
    lexer_free(lexer);
    return return_value;
}
