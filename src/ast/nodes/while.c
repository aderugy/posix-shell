#include "while.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "lexer/lexer.h"
#include "utils/logger.h"
/*
   rule_while = 'while' compound_list 'do' compound_list 'done'
*/
struct ast_while_node *ast_parse_while(struct lexer *lexer)
{
    struct token *tok = lexer_peek(lexer);
    if (!tok || tok->type != TOKEN_WORD || strcmp(tok->value.c, "while") != 0)
    {
        return NULL;
    }
    lexer_pop(lexer);
    free(tok);

    struct ast_while_node *ast = calloc(1, sizeof(struct ast_while_node));
    if (!ast)
    {
        errx(2, "out of memory");
    }

    ast->condition = ast_create(lexer, AST_CLIST);
    if (ast->condition == NULL)
    {
        errx(AST_PARSE_ERROR, "while: missing 1st clist");
    }

    tok = lexer_pop(lexer);
    if (!tok || tok->type != TOKEN_WORD || strcmp(tok->value.c, "do") != 0)
    {
        errx(2, "while: missing do token");
    }
    free(tok);

    ast->body = ast_create(lexer, AST_CLIST);
    if (ast->body == NULL)
    {
        errx(AST_PARSE_ERROR, "while: missing 2nd clist");
    }

    tok = lexer_pop(lexer);
    if (!tok || tok->type != TOKEN_WORD || strcmp(tok->value.c, "done") != 0)
    {
        errx(AST_PARSE_ERROR, "while: missing done token");
    }
    free(tok);

    return ast;
}

int ast_eval_while(struct ast_while_node *node, void **out,
                   __attribute((unused)) struct ast_eval_ctx *ctx)
{
    int ret_val;
    while (ast_eval(node->condition, out, NULL) == EXIT_SUCCESS)
    {
        ret_val = ast_eval(node->body, out, NULL);
        if (ret_val != EXIT_SUCCESS)
        {
            return ret_val;
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
