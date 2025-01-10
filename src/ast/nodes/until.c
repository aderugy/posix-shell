#include "until.h"

#include <err.h>
#include <stdlib.h>

#include "ast/ast.h"
#include "lexer/lexer.h"
#include "utils/logger.h"
/*
   rule_until = 'until' compound_list 'do' compound_list 'done'
*/
struct ast_until_node *ast_parse_until(struct lexer *lexer)
{
    struct token *tok = lexer_peek(lexer);
    if (!tok || tok->type != TOKEN_UNTIL)
    {
        return NULL;
    }
    lexer_pop(lexer);
    free(tok);

    struct ast_until_node *ast = calloc(1, sizeof(struct ast_until_node));
    if (!ast)
    {
        errx(2, "out of memory");
    }

    ast->condition = ast_create(lexer, AST_CLIST);
    if (ast->condition == NULL)
    {
        errx(2, "missing until condition");
    }

    tok = lexer_pop(lexer);
    if (!tok || tok->type != TOKEN_DO)
    {
        errx(2, "missing then token");
    }
    free(tok);

    ast->body = ast_create(lexer, AST_CLIST);
    if (ast->body == NULL)
    {
        errx(2, "missing done in until");
    }

    return ast;
}

int ast_eval_until(struct ast_until_node *node, void **out)
{
    int ret_val;
    while (!ast_eval(node->condition, out) == EXIT_SUCCESS)
    {
        ret_val = ast_eval(node->body, out);
        if (ret_val != EXIT_SUCCESS)
        {
            return ret_val;
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
