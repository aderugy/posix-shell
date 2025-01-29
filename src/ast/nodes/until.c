#include "until.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "lexer/lexer.h"
#include "utils/logger.h"
#include "utils/xalloc.h"
/*
   rule_until = 'until' compound_list 'do' compound_list 'done'
*/
struct ast_until_node *ast_parse_until(struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (!token_is_valid_keyword(token, "until"))
    {
        return NULL;
    }
    lexer_pop(lexer);

    struct ast_until_node *ast = xcalloc(1, sizeof(struct ast_until_node));

    ast->condition = ast_create(lexer, AST_CLIST);
    if (ast->condition == NULL)
    {
        lexer_error(lexer, "missing condition");
        goto error;
    }
    token_free(token);
    token = lexer_pop(lexer);
    if (!token_is_valid_keyword(token, "do"))
    {
        lexer_error(lexer, "expecting 'do'");
        goto error;
    }
    token_free(token);

    ast->body = ast_create(lexer, AST_CLIST);
    if (ast->body == NULL)
    {
        lexer_error(lexer, "expecting body");
        goto error;
    }

    token = lexer_pop(lexer);
    if (!token_is_valid_keyword(token, "done"))
    {
        lexer_error(lexer, "expecting 'done'");
        goto error;
    }
    token_free(token);

    return ast;

error:
    if (token)
    {
        token_free(token);
    }
    ast_free_until(ast);
    return NULL;
}

int ast_eval_until(struct ast_until_node *node, struct linked_list *out,
                   struct ast_eval_ctx *ctx)
{
    int ret_val;
    while (!(ast_eval(node->condition, out, ctx) == EXIT_SUCCESS))
    {
        ret_val = ast_eval(node->body, out, ctx);
        if (ret_val != EXIT_SUCCESS)
        {
            return ret_val;
        }
        if (ctx->break_count > 0)
        {
            ctx->break_count--;
            return ret_val;
        }
        if (ctx->continue_count > 1)
        {
            ctx->continue_count--;
            return ret_val;
        }
        else if (ctx->continue_count == 1)
        {
            ctx->continue_count--;
        }
    }
    return EXIT_SUCCESS;
}

void ast_free_until(struct ast_until_node *node)
{
    if (node)
    {
        ast_free(node->condition);
        ast_free(node->body);
        free(node);
    }
}

void ast_print_until(struct ast_until_node *node)
{
    logger("until ");
    ast_print(node->condition);

    logger(" then ");
    ast_print(node->body);
}
