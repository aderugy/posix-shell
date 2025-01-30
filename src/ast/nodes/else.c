#include "else.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "if.h"
#include "lexer/lexer.h"
#include "node.h"
#include "utils/logger.h"
#include "utils/xalloc.h"

/*
  else_clause =
    'else' compound_list
    | 'elif' compound_list 'then' compound_list [else_clause]
 */
static void poppin_tok(struct lexer *lexer, struct token **token)
{
    lexer_pop(lexer);
    token_free(*token);

    *token = NULL;
}

struct ast_else_node *ast_parse_else(struct lexer *lexer)
{
    struct ast_else_node *node = xcalloc(1, sizeof(struct ast_else_node));
    struct token *token = lexer_peek(lexer);

    if (token_is_valid_keyword(token, "else"))
    {
        lexer_pop(lexer);
        token_free(token);
        token = NULL;
        struct ast_node *body = ast_create(lexer, AST_CLIST);
        if (!body)
        {
            lexer_error(lexer, "else: expected body");
            goto error;
        }
        node->body = body;
        return node;
    }

    if (token_is_valid_keyword(token, "elif"))
    {
        poppin_tok(lexer, &token);

        struct ast_node *condition = ast_create(lexer, AST_CLIST);
        if (!condition)
        {
            lexer_error(lexer, "else: no condition");
            goto error;
        }
        token = lexer_peek(lexer);
        if (!token_is_valid_keyword(token, "then"))
        {
            lexer_error(lexer, "else: no then");
            goto error;
        }
        poppin_tok(lexer, &token);

        struct ast_node *body = ast_create(lexer, AST_CLIST);
        if (!body)
        {
            lexer_error(lexer, "else: no then");
            goto error;
        }

        struct ast_node *else_clause = ast_create(lexer, AST_ELSE);

        node->condition = condition;
        node->body = body;
        node->else_clause = else_clause;
        return node;
    }
    else
    {
        ast_free_else(node);
        return NULL;
    }
error:
    ast_free_else(node);
    if (token)
    {
        token_free(token);
    }
    return NULL;
}

int ast_eval_else(struct ast_else_node *node, struct linked_list *out,
                  struct ast_eval_ctx *ctx)
{
    if (node->condition == NULL) // else
    {
        return ast_eval(node->body, out, ctx);
    }
    else // ELIF
    {
        if (ast_eval(node->condition, out, ctx) == 0)
            return ast_eval(node->body, out, ctx);
        else
            return ast_eval(node->else_clause, out, ctx);
    }
}

void ast_free_else(struct ast_else_node *node)
{
    if (node->condition)
        ast_free(node->condition);
    if (node->else_clause)
        ast_free(node->else_clause);
    ast_free(node->body);
    free(node);
}

void ast_print_else(__attribute((unused)) struct ast_else_node *node)
{
    if (node->condition)
    {
        logger("elif ");
        ast_print(node->condition);
        logger(" then ");
        if (node->else_clause)
        {
            logger(" ");
            ast_print(node->else_clause);
        }
    }
    else
    {
        logger("else ");
        ast_print(node->body);
    }
}
