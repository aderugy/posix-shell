#include "while.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "expansion/expansion.h"
#include "lexer/lexer.h"
#include "utils/logger.h"
#include "utils/xalloc.h"
/*
   rule_while = 'while' compound_list 'do' compound_list 'done'
*/
struct ast_while_node *ast_parse_while(struct lexer *lexer)
{
    struct token *tok = lexer_peek(lexer);
    if (!reserved_word_check(tok) || strcmp(tok->value.c, "while") != 0)
    {
        return NULL;
    }
    token_free(lexer_pop(lexer));

    struct ast_while_node *ast = xcalloc(1, sizeof(struct ast_while_node));

    ast->condition = ast_create(lexer, AST_CLIST);
    if (ast->condition == NULL)
    {
        lexer_error(lexer, "expecting condition");
        goto error;
    }

    tok = lexer_pop(lexer);
    if (!reserved_word_check(tok) || strcmp(tok->value.c, "do") != 0)
    {
        lexer_error(lexer, "expecting 'do'");
        goto error;
    }
    token_free(tok);

    ast->body = ast_create(lexer, AST_CLIST);
    if (ast->body == NULL)
    {
        lexer_error(lexer, "expecting body");
        goto error;
    }

    tok = lexer_pop(lexer);
    if (!reserved_word_check(tok) || strcmp(tok->value.c, "done") != 0)
    {
        lexer_error(lexer, "expecting 'done'");
        goto error;
    }
    token_free(tok);

    return ast;

error:
    ast_free_while(ast);
    token_free(tok);
    return NULL;
}

int ast_eval_while(struct ast_while_node *node, void **out,
                   struct ast_eval_ctx *ctx)
{
    int ret_val;
    while (ast_eval(node->condition, out, ctx) == EXIT_SUCCESS)

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

void ast_free_while(struct ast_while_node *node)
{
    if (node)
    {
        ast_free(node->condition);
        ast_free(node->body);
        free(node);
    }
}

void ast_print_while(struct ast_while_node *node)
{
    logger("while ");
    ast_print(node->condition);

    logger(" then ");
    ast_print(node->body);
}
