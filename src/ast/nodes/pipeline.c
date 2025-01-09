#include "pipeline.h"

#include <err.h>
#include <stdlib.h>

#include "node.h"

struct ast_pipeline *ast_parse_pipeline(struct lexer *lexer)
{
    struct ast_node *command = ast_create(lexer, AST_COMMAND);
    if (!command)
    {
        return NULL;
    }

    struct ast_pipeline *node = calloc(1, sizeof(struct ast_pipeline));
    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    node->command = command;
    return node;
}

int ast_eval_pipeline(struct ast_pipeline *node, void **out)
{
    ast_eval(node->command, NULL);
}

void ast_free_pipeline(struct ast_pipeline *pipeline)
{
    ast_free(pipeline->command);
    free(pipeline);
}

void ast_print_pipeline(struct ast_pipeline *pipeline)
{
    ast_print(pipeline->command);
}
