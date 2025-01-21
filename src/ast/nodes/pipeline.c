#include "pipeline.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expansion/expansion.h"
#include "expansion/vars.h"
#include "lexer/lexer.h"
#include "node.h"
#include "utils/err_utils.h"
#include "utils/logger.h"
#include "utils/mypipe.h"
#include "utils/xalloc.h"

/*
 * pipeline = ['!'] command { '|' {'\n'} command } ;
 */

struct ast_pipeline *ast_parse_pipeline(struct lexer *lexer)
{
    struct ast_node *command = NULL;

    struct ast_pipeline *node = xcalloc(1, sizeof(struct ast_pipeline));
    node->commands = list_init();

    logger("Parse PIPELINE\n");
    struct token *token = lexer_peek(lexer);
    if (!token)
    {
        goto error;
    }

    if (token->type == TOKEN_WORD
        && (token->value.c[0] == '!' && strlen(token->value.c) == 1))
    {
        node->not = 1;
        lexer_pop(lexer);
        token_free(token);
        token = lexer_peek(lexer);
    }

    command = ast_create(lexer, AST_COMMAND);
    if (!command)
    {
        goto error;
    }
    list_append(node->commands, command);

    while ((token = lexer_peek(lexer)) && token->type == TOKEN_PIPE)
    {
        lexer_pop(lexer);
        token_free(token);
        token = lexer_peek(lexer);

        while (token->type == TOKEN_NEW_LINE)
        {
            lexer_pop(lexer);
            token_free(token);
            token = lexer_peek(lexer);
        }

        command = ast_create(lexer, AST_COMMAND);
        if (!command)
        {
            lexer_error(lexer, "expected command");
            goto error;
        }

        list_append(node->commands, command);
        token = lexer_peek(lexer);
    }

    logger("Exit PIPELINE (SUCCESS)\n");
    return node;

error:
    if (command)
    {
        ast_free(command);
    }

    ast_free_pipeline(node);
    logger("Exit PIPELINE (ERROR)\n");
    return NULL;
}

int ast_eval_pipeline(struct ast_pipeline *node, void **out,
                      struct ast_eval_ctx *ctx)
{
    int result;
    if (node->commands->size == 1)
    {
        result = ast_eval(list_get(node->commands, 0), out, ctx);
    }
    else
    {
        result = exec_pipeline(node->commands, ctx);
    }
    if (node->not== 1)
    {
        result = !result;
    }

    update_qm(ctx, result);
    return result;
}

void ast_free_pipeline(struct ast_pipeline *pipeline)
{
    list_free(pipeline->commands, (void (*)(void *))ast_free);
    free(pipeline);
}

void ast_print_pipeline(struct ast_pipeline *pipeline)
{
    for (size_t i = 0; i < pipeline->commands->size; i++)
    {
        ast_print(list_get(pipeline->commands, i));
    }
}
