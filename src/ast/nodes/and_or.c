#include "and_or.h"

#include <err.h>
#include <stdlib.h>

#include "ast/ast.h"
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

    struct ast_and_or_node *root = calloc(1, sizeof(struct ast_and_or_node));
    if (!root)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    root->type = NONE;
    root->left = pipeline;

    struct ast_and_or_node *node = root;

    struct token *tok = lexer_peek(lexer);
    while (tok->type == TOKEN_AND && tok->type == TOKEN_OR)
    {
        if (tok->type == TOKEN_AND)
        {
            node->type = AND;
        }
        else
        {
            node->type = OR;
        }
        free(lexer_pop(lexer));

        node->right = calloc(1, sizeof(struct ast_node));
        node = node->right;
        node->type = NONE;

        while (lexer_peek(lexer)->type == TOKEN_NEW_LINE)
        {
            free(lexer_pop(lexer));
        }

        node->left = ast_create(lexer, AST_PIPELINE);
        if (node->left == NULL)
        {
            errx(AST_PARSE_ERROR, "and_or: 2nd pipeline did not match");
        }
    }

    return root;
}

int ast_eval_and_or(struct ast_and_or_node *node, void **out)
{
    int ret_val = ast_eval(node->left, out);
    if (node->type == AND && ret_val == EXIT_SUCCESS)
    {
        return ast_eval_and_or(node->right, out);
    }
    if (node->type == OR && ret_val != EXIT_SUCCESS)
    {
        return ast_eval_and_or(node->right, out);
    }

    return ret_val;
}

void ast_free_and_or(struct ast_and_or_node *node)
{
    if (node)
    {
        ast_free(node->left);
        ast_free_and_or(node->right);
        free(node);
    }
}

void ast_print_and_or(struct ast_and_or_node *node)
{
    if (!node->right)
    {
        return;
    }

    ast_print(node->left);

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
