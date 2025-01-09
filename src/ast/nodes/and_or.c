#include "and_or.h"

#include <err.h>
#include <stdlib.h>

#include "lexer/token.h"
#include "node.h"
#include "utils/logger.h"

struct ast_and_or_node *ast_parse_and_or(struct lexer *lexer)
{
    struct ast_node *pipeline = ast_create(lexer, AST_PIPELINE);
    if (!pipeline)
    {
        return NULL;
    }

    struct ast_and_or_node *node = calloc(1, sizeof(struct ast_and_or_node));
    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    node->left = pipeline;
    return node;
}

int ast_eval_and_or(struct ast_and_or_node *node, void **out)
{
    int val = ast_eval(node->left, out);
    /*    if (val == 0 && node->is_and)
        {
            return val && ast_eval(node->right, out);
        }
        else if (val != 0 && !node->is_and)
        {
            return ast_eval(node->right, out);
        }*/
    return val;
}

void ast_free_and_or(struct ast_and_or_node *node)
{
    ast_free(node->left);
    free(node);
}

void ast_print_and_or(__attribute((unused)) struct ast_and_or_node *node)
{
    if (!node->left)
    {
        return;
    }

    ast_print(node->left);

    if (node->right)
    {
        if (node->is_and)
        {
            logger(" && ");
        }
        else
        {
            logger(" || ");
        }

        ast_print(node->right);
    }
}
