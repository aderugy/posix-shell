#ifndef IONUMBER_H
#define IONUMBER_H

#include "lexer/lexer.h"

struct ast_ionumber
{
    long long value;
};

struct ast_ionumber *ast_parse_ionumber(struct lexer *lexer);
int ast_eval_ionumber(struct ast_ionumber *node, void **out);
void ast_free_ionumber(struct ast_ionumber *node);
void ast_print_ionumber(struct ast_ionumber *node);

#endif // !IONUMBER_H
