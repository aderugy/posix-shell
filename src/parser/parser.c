#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "ast/ast.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "utils/logger.h"

struct ast_node *parse(__attribute__((unused)) struct lexer *lexer)
{
    return NULL;
}

struct ast_node *parse_expr(__attribute__((unused)) struct lexer *lexer)
{
    return NULL;
}

struct ast_node *
parse_simple_command(__attribute__((unused)) struct lexer *lexer)
{
    return NULL;
}
