#include "for.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "lexer/lexer.h"
#include "utils/logger.h"
/*
   rule_for = 'for' WORD ( [';'] | [ {'\n'} 'in' { WORD } ( ';' | '\n' ) ] )
              {'\n'} 'do' compound_list 'done' ;
*/
struct ast_for_node *ast_parse_for(struct lexer *lexer)
{
    struct token *tok = lexer_peek(lexer);
    if (!tok || tok->type != TOKEN_WORD || strcmp(tok->value.c, "for") != 0)
    {
        return NULL;
    }
    lexer_pop(lexer);
    free(tok);

    struct ast_for_node *ast = calloc(1, sizeof(struct ast_for_node));
    if (!ast)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    ast_free_for(ast);
    errx(AST_PARSE_ERROR, "Unimplemented ast_parse_for");
}

int ast_eval_for(__attribute((unused)) struct ast_for_node *node,
                 __attribute((unused)) void **out,
                 __attribute((unused)) struct ast_eval_ctx *ctx)
{
    errx(EXIT_FAILURE, "Unimplemented ast_eval_for");
}

void ast_free_for(struct ast_for_node *node)
{
    if (node)
    {
        ast_free(node->elt);
        ast_free(node->in);
        ast_free(node->body);
        free(node);
    }
}

void ast_print_for(struct ast_for_node *node)
{
    logger("for ");
    ast_print(node->elt);

    logger("in ");
    ast_print(node->in);

    logger("do ");
    ast_print(node->body);
}
