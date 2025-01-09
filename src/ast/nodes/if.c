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

int ast_eval_if(__attribute((unused)) struct ast_if_node *node,
                __attribute((unused)) void **out)
{
    errx(EXIT_FAILURE, "not implemented");
}

void ast_free_if(__attribute((unused)) struct ast_if_node *node)
{
    errx(EXIT_FAILURE, "not implemented");
}

void ast_print_if(__attribute((unused)) struct ast_if_node *node)
{
    errx(EXIT_FAILURE, "not implemented");
}
