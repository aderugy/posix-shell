#include "command.h"

#include <err.h>
#include <stdlib.h>

#include "node.h"

struct ast_cmd *ast_parse_cmd(struct lexer *lexer)
{
    struct ast_cmd *node = NULL;
    struct ast_node *simple_cmd = ast_create(lexer, AST_SIMPLE_COMMAND);

    if (simple_cmd)
    {
        node = calloc(1, sizeof(struct ast_cmd));
        if (!node)
        {
            errx(EXIT_FAILURE, "out of memory");
        }

        node->type = SIMPLE_CMD;
        node->cmd = simple_cmd;
        return node;
    }
    struct ast_node *shell_cmd = ast_create(lexer, AST_SHELL_COMMAND);

    if (!shell_cmd)
    {
        return NULL;
    }

    node = calloc(1, sizeof(struct ast_cmd));
    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    node->type = SHELL_CMD;
    node->cmd = shell_cmd;
    return node;
}

void ast_free_cmd(struct ast_cmd *node)
{
    ast_free(node->cmd);
    free(node);
}

int ast_eval_cmd(struct ast_cmd *node, void **out)
{
    return ast_eval(node->cmd, out);
}

void ast_print_cmd(struct ast_cmd *node)
{
    ast_print(node->cmd);
}
