#include "else.h"

#include <err.h>
#include <stdlib.h>

#include "lexer/lexer.h"
#include "node.h"

/*
  else_clause =
    'else' compound_list
    | 'elif' compound_list 'then' compound_list [else_clause]
 */

struct ast_else_node *ast_parse_else(struct lexer *lexer)
{
    ast_else_node node =
        calloc(1, sizeof(struct ast_else_node)) struct token *token =
            lexer_peek(lexer);

    if (token == TOKEN_ELSE)
    {
        lexer_pop(lexer);
        free(token);
        struct ast_node body = ast_create(lexer, AST_CLIST);
        if (!body)
            return NULL;
        node->body = body;
    }

    else if (token == TOKEN_ELIF)
    {
        lexer_pop(lexer);
        free(token);

        struct ast_node condition = ast_create(lexer, AST_CLIST);
        if (!condition)
            return NULL;

        token = lexer_peek(lexer);
        if (token != TOKEN_THEN)
            return NULL;
        lexer_pop(lexer);
        free(token);

        struct ast_node body = ast_create(lexer, AST_CLIST);
        if (!body)
            return NULL;

        struct ast_node else_clause = ast_create(lexer, AST_ELSE);

        node->condition = condition;
        node->body = body;
        node->else_clause = else_clause;
    }
    else
    {
        return NULL;
    }
    return node;
}

int ast_eval_else(struct ast_else_node *node, out)
{
    if (node->condition == NULL) // else
    {
        return ast_eval(node->body, out);
    }
    else // ELIF
    {
        if (ast_eval(node->condition, out) == 0)
            return ast_eval(node->body, out);
        else
            return ast_eval(node->else_clause, out);
    }
}

void ast_free_else(struct ast_else_node *node)
{
    if (node->condition)
        ast_free(node->condition);
    if (node->else_clause)
        ast_free(node->else_clause);
    ast_free(node->body);
}

void ast_print_else(__attribute((unused)) struct ast_else_node *node)
{
    logger("ELSE CLAUSE");
}
