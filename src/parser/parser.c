#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "ast/ast.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "utils/logger.h"

// forward declaration
struct ast_node *parse_input(__attribute__((unused)) struct lexer *lexer);
struct ast_node *parse_list(__attribute__((unused)) struct lexer *lexer);
struct ast_node *parse_and_or(__attribute__((unused)) struct lexer *lexer);
struct ast_node *parse_pipeline(__attribute__((unused)) struct lexer *lexer);
struct ast_node *parse_command(__attribute__((unused)) struct lexer *lexer);
struct ast_node *parse_rule_if(__attribute__((unused)) struct lexer *lexer);
struct ast_node *parse_element(__attribute__((unused)) struct lexer *lexer);
struct ast_node *parse_else_clause(__attribute__((unused)) struct lexer *lexer);
struct ast_node *
parse_simple_command(__attribute__((unused)) struct lexer *lexer);
struct ast_node *
parse_shell_command(__attribute__((unused)) struct lexer *lexer);
struct ast_node *
parse_compound_list(__attribute__((unused)) struct lexer *lexer);

struct ast_node *parse(__attribute__((unused)) struct lexer *lexer)
{
    return NULL;
}
/* just a way to reduce the number of lines per functions
   limit code repetitions
   @return: always returns NULL
*/
struct ast_node *exit_shortcut(char message[], struct ast_node *ast)
{
    perror(message);
    ast_free(ast);
    return NULL;
}
/*
    input = list '\n'
          | list EOF
          | '\n'
          | EOF
    upon error : returns NULL and prints stderr
*/
struct ast_node *parse_input(struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    struct ast_node *root = NULL;
    switch (tok.type)
    {
    case TOKEN_EOF:
    case TOKEN_NEW_LINE:
        return NULL;
    default:
        tok = lexer_pop(lexer);
        root = parse_list(lexer);
        break;
    }
    tok = lexer_pop(lexer);
    if (tok.type != TOKEN_EOF && tok.type != TOKEN_NEW_LINE)
    {
        perror("Unexpected token at the end of input parsing");
        ast_free(root);
        return NULL;
    }
    return root;
}

/*
    list = and_or { ';' and_or } [ ';' ]
*/
struct ast_node *parse_list(__attribute__((unused)) struct lexer *lexer)
{
    logger(" -- parse list\n");
    // TODO implement how we build the ast with AND_OR nodes
    return NULL;
}
// and_or = pipeline
struct ast_node *parse_and_or(struct lexer *lexer)
{
    return parse_pipeline(lexer);
}
// pipeline = command
struct ast_node *parse_pipeline(struct lexer *lexer)
{
    return parse_command(lexer);
}
/*
command = simple_command
           | shell_command
*/
struct ast_node *parse_command(struct lexer *lexer)
{
    struct ast_node *ast = parse_simple_command(lexer);
    if (ast == NULL)
    {
        return parse_shell_command(lexer);
    }
    return ast;
}
// simple_command = WORD { element }
struct ast_node *parse_simple_command(struct lexer *lexer)
{
    struct token tok = lexer_pop(lexer);
    if (tok.type != TOKEN_WORD)
    {
        perror("Unexpected token in simple_command. Expected WORD");
        return NULL;
    }
    // TODO build and return the simple_command node
    return NULL;
}
// shell_command = rule_if
struct ast_node *parse_shell_command(struct lexer *lexer)
{
    return parse_rule_if(lexer);
}
/*
    rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi'
*/
struct ast_node *parse_rule_if(struct lexer *lexer)
{
    struct token tok = lexer_pop(lexer);
    if (tok.type != TOKEN_IF)
    {
        perror("Unexpected token in rule_if. Expected IF");
        return NULL;
    }
    struct ast_node *ast = new_ast(IF);
    ast->value.if_node->condition = parse_compound_list(lexer);
    // check for ast->condition being NULL
    if (ast->value.if_node->condition == NULL)
    {
        return exit_shortcut("Internal error in compound_list", ast);
    }
    tok = lexer_pop(lexer);
    if (tok.type != TOKEN_THEN)
    {
        return exit_shortcut("Unexpected token in rule_if. Expected THEN", ast);
    }
    ast->value.if_node->body = parse_compound_list(lexer);
    // check for ast->body being NULL
    if (ast->value.if_node->body == NULL)
    {
        return exit_shortcut("Internal error in compound_list", ast);
    }
    tok = lexer_peek(lexer);
    if (tok.type == TOKEN_ELSE || tok.type == TOKEN_ELIF)
    {
        ast->value.if_node->else_clause = parse_compound_list(lexer);
        // !!! maybe check for ast->else_clause being NULL
        if (ast->value.if_node->else_clause == NULL)
        {
            return exit_shortcut("Internal error in compound_list", ast);
        }
    }
    tok = lexer_pop(lexer);
    if (tok.type != TOKEN_FI)
    {
        return exit_shortcut("Unexpected token in rule_if. Expected FI", ast);
    }
    return ast;
}
/* compound_list =
    { '\n' } and_or { ( ';' | '\n' ) { '\n' } and_or } [';'] {'\n'}
*/
struct ast_node *
parse_compound_list(__attribute__((unused)) struct lexer *lexer)
{
    // TODO
    return NULL;
}
/*
else_clause =
             'else' compound_list
           | 'elif' compound_list 'then' compound_list [else_clause]
*/
struct ast_node *parse_else_clause(__attribute__((unused)) struct lexer *lexer)
{
    // TODO
    return NULL;
}
