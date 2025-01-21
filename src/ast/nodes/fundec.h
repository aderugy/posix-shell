#ifndef FUNDEC_H
#define FUNDEC_H

#include "lexer/lexer.h"
#include "node.h"

struct ast_fundec
{
    char *name;
    struct ast_node *ast_node; // shell_command
};

struct ast_fundec *ast_parse_fundec(struct lexer *lexer);
int ast_eval_fundec(struct ast_fundec *cmd, struct linked_list *ptr,
                    __attribute((unused)) struct ast_eval_ctx *ctx);
void ast_free_fundec(struct ast_fundec *cmd);
void ast_print_fundec(struct ast_fundec *cmd);

#endif // !FUNDEC_H
