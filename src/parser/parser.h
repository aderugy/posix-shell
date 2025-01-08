#ifndef PARSER_H
#define PARSER_H

#include "lexer/lexer.h"
struct ast_node *parse_simple_command(struct lexer *lexer);
struct ast_node *parse_expr(struct lexer *lexer);
struct ast_node *parse(struct lexer *lexer);
#endif /* ! PARSER_H */
