#include "and_or.h"

#include <err.h>
#include <stdlib.h>

#include "ast/ast.h"
#include "lexer/token.h"
#include "utils/err_utils.h"
#include "utils/logger.h"
#include "utils/xalloc.h"

struct ast_and_or_node *ast_parse_and_or(struct lexer *lexer)
{
    struct ast_and_or_node *root = xcalloc(1, sizeof(struct ast_and_or_node));
    root->type = NONE;

    logger("Parse AND_OR\n");
    struct ast_node *pipeline = ast_create(lexer, AST_PIPELINE);
    if (!pipeline)
    {
        goto error;
    }
    root->left = pipeline;

    struct ast_and_or_node *node = NULL;
    struct token *tok = lexer_peek(lexer);
    while (tok && (tok->type == TOKEN_AND || tok->type == TOKEN_OR))
    {
        node = root;

        root = xcalloc(1, sizeof(struct ast_and_or_node));
        root->right = node;

        if (tok->type == TOKEN_AND)
        {
            root->type = AND;
        }
        else
        {
            root->type = OR;
        }
        free(lexer_pop(lexer));

        while (lexer_peek(lexer)->type == TOKEN_NEW_LINE)
        {
            free(lexer_pop(lexer));
        }

        root->left = ast_create(lexer, AST_PIPELINE);
        if (node->left == NULL)
        {
            errx(2, "ast_parse_and_or: second pipeline did not match");
            goto error;
        }

        tok = lexer_peek(lexer);
    }

    logger("Exit AND_OR (SUCCESS)\n");
    return root;

error:
    ast_free_and_or(root);
    logger("Exit AND_OR (ERROR)\n");
    return NULL;
}

int ast_eval_and_or(struct ast_and_or_node *node, void **out,
                    struct ast_eval_ctx *ctx)
{
    if (!node)
    {
        return EXIT_SUCCESS;
    }

    if (!node->right)
    {
        return ast_eval(node->left, out, ctx);
    }

    int ret_val = ast_eval_and_or(node->right, out, ctx);
    if (node->type == AND && ret_val == EXIT_SUCCESS)
    {
        return ast_eval(node->left, out, ctx);
    }
    if (node->type == OR && ret_val != EXIT_SUCCESS)
    {
        return ast_eval(node->left, out, ctx);
    }

    return ret_val;
}

void ast_free_and_or(struct ast_and_or_node *node)
{
    if (node)
    {
        if (node->left)
        {
            ast_free(node->left);
        }

        if (node->right)
        {
            ast_free_and_or(node->right);
        }

        free(node);
    }
}

void ast_print_and_or(struct ast_and_or_node *node)
{
    if (node->left)
    {
        ast_print(node->left);
    }

    if (!node->right)
    {
        return;
    }

    if (node->right)
    {
        if (node->type == AND)
        {
            logger(" && ");
        }
        else if (node->type == OR)
        {
            logger(" || ");
        }

        ast_print_and_or(node->right);
    }
}
