#include "if.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "lexer/lexer.h"
#include "node.h"
#include "utils/logger.h"
#include "utils/xalloc.h"
/*
   rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi'
*/
struct ast_if_node *ast_parse_if(struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (!(TOKEN_OK) || strcmp(token->value.c, "if") != 0)
    {
        return NULL;
    }
    lexer_pop(lexer);
    token_free(token);

    struct ast_if_node *ast = xcalloc(1, sizeof(struct ast_if_node));

    ast->condition = ast_create(lexer, AST_CLIST);
    if (ast->condition == NULL)
    {
        errx(2, "missing if condition");
    }

    logger("PARSE IF\n");
    token = lexer_pop(lexer);
    if (!(TOKEN_OK) || strcmp(token->value.c, "then") != 0)
    {
        errx(2, "missing then token");
    }
    token_free(token);

    struct ast_node *body = ast_create(lexer, AST_CLIST);
    if (body == NULL)
    {
        errx(2, "missing if body");
    }
    ast->body = body;

    ast->else_clause = ast_create(lexer, AST_ELSE);

    token = lexer_pop(lexer);
    if (!(TOKEN_OK) || strcmp(token->value.c, "fi") != 0)
    {
        errx(2, "missing fi");
    }

    token_free(token);
    logger("PARSE IF (SUCCESS)\n");
    return ast;
}

int ast_eval_if(struct ast_if_node *node, struct linked_list *out,
                struct ast_eval_ctx *ctx)
{
    int value = ast_eval(node->condition, out, ctx);

    if (value == 0)
    {
        return ast_eval(node->body, out, ctx);
    }
    if (node->else_clause)
        return ast_eval(node->else_clause, out, ctx);
    return 0;
}

void ast_free_if(struct ast_if_node *node)
{
    if (node)
    {
        ast_free(node->condition);
        ast_free(node->body);
        if (node->else_clause)
        {
            ast_free(node->else_clause);
        }
        free(node);
    }
}

void ast_print_if(struct ast_if_node *node)
{
    logger("if { ");
    ast_print(node->condition);
    logger(" } then { ");
    ast_print(node->body);

    if (node->else_clause)
    {
        logger(" ");
        ast_print(node->else_clause);
    }
    logger(" }");
}
