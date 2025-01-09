#include "command.h"

#include <err.h>
#include <stdlib.h>

#include "node.h"

struct ast_cmd *ast_parse_cmd(struct lexer *lexer)
{
    struct ast_node *simple_cmd = ast_create(lexer, AST_SIMPLE_COMMAND);
    if (!simple_cmd)
    {
        return NULL;
    }

    struct ast_cmd *node = calloc(1, sizeof(struct ast_cmd));
    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }
    node->simple_cmd = simple_cmd;

    return node;
}

void ast_free_cmd(struct ast_cmd *node)
{
    ast_free(node->simple_cmd);
    free(node);
}

int ast_eval_cmd(struct ast_cmd *node, void **out)
{
    return ast_eval(node->simple_cmd, out);
}
