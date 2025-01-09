#include "if.h"

#include <err.h>
#include <stdlib.h>

#include "lexer/lexer.h"
/*
   rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi'
*/
struct ast_if_node *ast_parse_if(struct lexer *lexer)
{
    struct token *tok = lexer_peek(lexer);
    if (tok->type != TOKEN_IF)
    {
        free(tok);
        return NULL;
    }
    lexer_pop(lexer);
    free(tok);

    struct ast_if_node *ast = calloc(1, sizeof(struct ast_if_node));
    if (!ast)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    ast->condition = ast_create(lexer, AST_CLIST);
    if (ast->condition == NULL)
    {
        errx(EXIT_FAILURE, "Internal error in rule if.");
    }

    tok = lexer_pop(lexer);
    if (tok->type != TOKEN_THEN)
    {
        errx(EXIT_FAILURE, "Unexpected token in rule_if. Expected THEN");
    }
    free(tok);

    ast->body = ast_create(lexer, AST_CLIST);
    if (ast->body == NULL)
    {
        errx(EXIT_FAILURE, "Internal error in rule if.");
    }

    ast->else_clause = ast_create(lexer, AST_ELSE);

    tok = lexer_pop(lexer);
    if (tok->type != TOKEN_FI)
    {
        errx(EXIT_FAILURE, "Unexpected token in rule_if. Expected FI");
    }
    free(tok);

    return ast;
}

int ast_eval_if(struct ast_if_node *node, void **out)
{
    errx(EXIT_FAILURE, "not implemented");
    if (ast_eval(node->condition, out) == 0)
    {
        return ast_eval(node->body, out);
    }
    return ast_eval(node->else_clause, out);
}

void ast_free_if(struct ast_if_node *node)
{
    if (node)
    {
        ast_free(node->condition);
        ast_free(node->body);
        ast_free(node->else_clause);
        free(node);
    }
}

void ast_print_if(struct ast_if_node *node)
{
    if (node)
    {
        ast_print(node->condition);
        ast_print(node->body);
        ast_print(node->else_clause);
    }
}
