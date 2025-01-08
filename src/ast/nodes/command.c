#include "command.h"

#include <err.h>
#include <stdlib.h>

#include "node.h"
#include "simple_command.h"

struct ast_cmd *ast_parse_cmd(struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type != LIST)
    {
        return NULL;
    }

    lexer_pop(lexer);

    struct ast_cmd *node = calloc(1, sizeof(struct ast_cmd));

    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    node->simple_cmd = ast_create(lexer, SIMPLE_COMMAND);
    if (!node->simple_cmd)
    {
        errx(EXIT_FAILURE, "error parse cmd");
    }
}

void ast_free_cmd(struct ast_cmd *node)
{
    ast_free_simple_cmd(node->simple_cmd);
    free(node);
}

int ast_eval_cmd(struct ast_cmd *node, void **out)
{
    ast_eval_simple_cmd(node->simple_cmd, out);
}
