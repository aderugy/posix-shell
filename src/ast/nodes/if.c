#include "if.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "lexer/lexer.h"
#include "node.h"
#include "utils/logger.h"
/*
   rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi'
*/
struct ast_if_node *ast_parse_if(struct lexer *lexer)
{
    struct token *tok = lexer_peek(lexer);
    if (!tok || !tok->value.c || strcmp(tok->value.c, "if") != 0)
    {
        return NULL;
    }
    lexer_pop(lexer);
    free(tok);

    struct ast_if_node *ast = calloc(1, sizeof(struct ast_if_node));
    if (!ast)
    {
        errx(2, "out of memory");
    }

    ast->condition = ast_create(lexer, AST_CLIST);
    if (ast->condition == NULL)
    {
        errx(2, "missing if condition");
    }

    logger("\tif.c : SUCCESSFULLY found IF\n");

    tok = lexer_pop(lexer);
    if (!tok || tok->type != TOKEN_THEN)
    {
        errx(2, "missing then token");
    }

    logger("\tif.c : SUCCESSFULLY found THEN\n");
    free(tok);

    struct ast_node *body = ast_create(lexer, AST_CLIST);
    if (body == NULL)
    {
        errx(2, "missing if body");
    }
    ast->body = body;
    logger("\tif.c : SUCCESSFULLY create body\n");

    ast->else_clause = ast_create(lexer, AST_ELSE);
    if (ast->else_clause)
    {
        logger("SUCCESSFULLY create else clause\n");
    }
    else
    {
        logger("SUCCESSFULLY NOT create else clause\n");
    }
    tok = lexer_pop(lexer);
    if (tok->value.c)
    {
        logger("\tjules token : %s\n", tok->value.c);
    }

    if (!tok || !tok->value.c || strcmp(tok->value.c, "fi") != 0)
    {
        errx(2, "missing fi");
    }
    logger("SUCCESSFULLY found FI\n");
    free(tok);

    return ast;
}

int ast_eval_if(struct ast_if_node *node, void **out)
{
    int value = ast_eval(node->condition, out);

    if (value == 0)
    {
        return ast_eval(node->body, out);
    }
    if (node->else_clause)
        return ast_eval(node->else_clause, out);
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
