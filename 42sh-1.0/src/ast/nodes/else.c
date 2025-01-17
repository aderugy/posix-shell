#include "else.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "if.h"
#include "lexer/lexer.h"
#include "node.h"
#include "utils/logger.h"

/*
  else_clause =
    'else' compound_list
    | 'elif' compound_list 'then' compound_list [else_clause]
 */

struct ast_else_node *ast_parse_else(struct lexer *lexer)
{
    struct ast_else_node *node = calloc(1, sizeof(struct ast_else_node));
    struct token *token = lexer_peek(lexer);

    logger("\tParsing ELSE_CLAUSE\n");

    if (token->type == TOKEN_WORD && strcmp(token->value.c, "else") == 0)
    {
        lexer_pop(lexer);
        free(token->value.c);
        free(token);
        struct ast_node *body = ast_create(lexer, AST_CLIST);
        if (!body)
            return NULL;
        node->body = body;
        return node;
    }

    else if (token->type == TOKEN_WORD && strcmp(token->value.c, "elif") == 0)
    {
        lexer_pop(lexer);
        free(token->value.c);
        free(token);

        struct ast_node *condition = ast_create(lexer, AST_CLIST);
        if (!condition)
            return NULL;

        token = lexer_peek(lexer);
        if (token->type != TOKEN_WORD || strcmp(token->value.c, "then") != 0)
            return NULL;
        lexer_pop(lexer);
        free(token->value.c);
        free(token);

        struct ast_node *body = ast_create(lexer, AST_CLIST);
        if (!body)
            return NULL;

        struct ast_node *else_clause = ast_create(lexer, AST_ELSE);

        node->condition = condition;
        node->body = body;
        node->else_clause = else_clause;
        return node;
    }
    else
    {
        free(node);
        return NULL;
    }
}

int ast_eval_else(struct ast_else_node *node, void **out,
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
