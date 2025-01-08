#include "if.h"

#include <err.h>
#include <stdlib.h>

#include "node.h"

struct ast_node *parse_rule_if(struct lexer *lexer)
{
    struct token tok = lexer_pop(lexer);
    if (tok.type != TOKEN_IF)
    {
        errx(1, "Unexpected token in rule_if. Expected IF");
        return NULL;
    }

    struct ast_node *ast = new_ast(IF);
    ast->value.if_node->condition = parse_compound_list(lexer);

    // check for ast->condition being NULL
    if (ast->value.if_node->condition == NULL)
    {
        errx(1, "Unexpected token in rule_if. Expected compound_list");
    }

    tok = lexer_pop(lexer);
    if (tok.type != TOKEN_THEN)
    {
        errx(1, "Unexpected token in rule_if. Expected");
    }

    ast->value.if_node->body = parse_compound_list(lexer);

    // check for ast->body being NULL
    if (ast->value.if_node->body == NULL)
    {
        errx(1, "Unexpected token in rule_if. Expected compound_list");
    }

    tok = lexer_peek(lexer);
    if (tok.type == TOKEN_ELSE || tok.type == TOKEN_ELIF)
    {
        ast->value.if_node->else_clause = parse_compound_list(lexer);

        // !!! maybe check for ast->else_clause being NULL
        if (ast->value.if_node->else_clause == NULL)
        {
            errx(1, "Unexpected token in rule_if. Expected compound_list");
        }
    }

    tok = lexer_pop(lexer);
    if (tok.type != TOKEN_FI)
    {
        errx(1, "Unexpected token in rule_if. Expected FI");
    }

    return ast;
}
