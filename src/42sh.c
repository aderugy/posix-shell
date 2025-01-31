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
#include "lexer/shard.h"
#include "lexer/splitter.h"
#include "streams/streams.h"
#include "utils/logger.h"

static struct option l_opts[] = { { "verbose", no_argument, 0, 'v' },
                                  { "comput", required_argument, 0, 'c' },
                                  { "test-token", no_argument, 0, 't' },

                                  { 0, 0, 0, 0 } };

int hs24(struct stream *stream, struct ast_eval_ctx *ctx)
{
    struct lexer *lexer = lexer_create(stream);

    int return_value;
    struct ast_node *node;

    /*
     * Process input line by line (AST_INPUT after AST_INPUT)
     */
    do
    {
        node = ast_create(lexer, AST_INPUT);
        if (!node)
        {
            warnx("42sh: Syntax error");
            return_value = 2;
            break;
        }

        ast_print(node);
        return_value = ast_eval(node, NULL, ctx);
        ast_free(node);
    } while (!lexer->eof && !lexer->error && node && !(return_value));

    if (lexer->error)
    {
        warnx("Syntax error");
        return_value = 2;
    }

    lexer_free(lexer);
    return return_value;
}

static int sub_main(struct stream *stream, struct ast_eval_ctx *ctx,
                    int nb_args)
{
    ctx_init_local_dollar(ctx);
    ctx_init_local_hashtag(nb_args, ctx);
    ctx_init_local_UID(ctx);
    ctx_update_local_qm(ctx, 0);
    register_commands();

    int return_value = hs24(stream, ctx);

    ctx_free(ctx);
    unregister_commands();

    return return_value;
}

/*static int main_shards(struct stream *stream, struct ast_eval_ctx *ctx)
{
    ctx_free(ctx);

    struct shard *shard;
    struct splitter_ctx *spltctx = splitter_ctx_init(stream);
    while ((shard = splitter_pop(spltctx)))
    {
        shard_print(shard);
        shard_free(shard);
    }
    splitter_ctx_free(spltctx);
    return 0;
}*/

/*static int main_tokens(struct stream *stream, struct ast_eval_ctx *ctx)
{
    struct lexer *lexer = lexer_create(stream);
    struct token *token;
    while ((token = lexer_pop(lexer)))
    {
        token_print(token);
        token_free(token);
    }
    lexer_free(lexer);
    ctx_free(ctx);
    return 0;
}*/

int main(int argc, char *argv[])
{
    int c;
    int opt_idx = 0;
    struct stream *stream = NULL;
    struct ast_eval_ctx *ctx = ctx_init();

    /*bool disp_lex = false;
    bool disp_shards = false;*/
    while ((c = getopt_long(argc, argv, "vc:ts", l_opts, &opt_idx)) != -1)
    {
        switch (c)
        {
        case 'v':
            logger(NULL, NULL);
            break;
        case 'c':
            stream = stream_from_str(optarg);
            ctx->stream_type = STREAM_FROM_STR;
            break;
        case 's':
            logger(NULL, NULL);
            // disp_shards = true;
            break;
        case 't':
            logger(NULL, NULL);
            // disp_lex = true;
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
            ctx->stream_type = STREAM_FROM_FILE;
        }
        else
        {
            stream = stream_from_stream(stdin);
            ctx->stream_type = STREAM_FROM_STREAM;
        }
    }

    if (!stream)
    {
        errx(1, "stream error");
    }

    /*disp_shards)
    {
        return main_shards(stream, ctx);
    }

    if (disp_lex)
    {
        return main_tokens(stream, ctx);
    }*/

    int nb_args = ctx_init_local_args(argc, argv, ctx);

    return sub_main(stream, ctx, nb_args);
}
