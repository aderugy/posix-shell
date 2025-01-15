#include "pipeline.h"

#include <err.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
#include "node.h"
#include "utils/logger.h"
#include "utils/mypipe.h"

/*
 * pipeline = ['!'] command { '|' {'\n'} command } ;
 */

struct ast_pipeline *ast_parse_pipeline(struct lexer *lexer)
{
    logger("Parse PIPELINE\n");
    struct ast_pipeline *node = calloc(1, sizeof(struct ast_pipeline));
    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }
    struct token *token = lexer_peek(lexer);
    if (!token)
    {
        free(node);
        logger("token NULL\n");
        logger("Exit PIPELINE\n");
        return NULL;
    }
    if (token->type == TOKEN_WORD
        && (token->value.c[0] == '!' && strlen(token->value.c) == 1))
    {
        node->not = 1;
        lexer_pop(lexer);
        token_free(token);
        token = lexer_peek(lexer);
    }
    struct ast_node *command = ast_create(lexer, AST_COMMAND);
    if (!command)
    {
        logger("%i", token->type);
        logger("pipeline: first command did not match\n");
        free(node);
        logger("Exit PIPELINE\n");
        return NULL;
    }

    token = lexer_peek(lexer);

    node->commands = list_init();
    list_append(node->commands, command);

    while (token->type == TOKEN_PIPE)
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

        struct ast_node *command = ast_create(lexer, AST_COMMAND);
        if (!command)
        {
            logger("pipeline: second command did not match\n");
            ast_free_pipeline(node);
            logger("Exit PIPELINE\n");
            return NULL;
        }

        list_append(node->commands, command);
        token = lexer_peek(lexer);
    }
    logger("Exit PIPELINE\n");
    return node;
}

int ast_eval_pipeline(struct ast_pipeline *node, void **out,
                      __attribute((unused)) struct ast_eval_ctx *ctx)
{
    int result;
    if (node->commands->size == 1)
        result = ast_eval(list_get(node->commands, 0), out, NULL);
    else
        result = exec_pipeline(node->commands);
    if (node->not == 1)
        return !result;
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
