#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include "mbtstr/str.h"
#include "parser/parser.h"

void print_ast()
{
    printf("ast !!!\n");
}

void simple_command_node_free(struct simple_command_node *simple_command_node)
{
    mbt_str_free(simple_command_node->command_name);
    for(size_t i =0; i<simple_command_node->elements_len;i++)
    {
        mbt_str_free((simple_command_node->elements)[i]);
    } 
    free(simple_command_node->elements);
}

void if_node_free(struct if_node *if_node)
{
    ast_free(if_node->condition);
    ast_free(if_node->body);
    ast_free(if_node->else_clause);
    free(if_node);
}

void ast_free(struct ast_node *node)
{
    if(node->type == IF)
    {
        if_node_free(node->value.if_node);
    }
    else if(node->type == SIMPLE_COMMAND)
    {
        simple_command_node_free(node->value.simple_command);
    }
    
    free(node);
}
