#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

#include "parser/parser.h"

void print_ast()
{
    printf("ast !!!\n");
}

void ast_free(__attribute__((unused)) struct ast_node *node)
{}
