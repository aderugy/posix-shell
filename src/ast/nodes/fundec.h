#ifndef FUNDEC_H
#define FUNDEC_H

#include <stdbool.h>

#include "lexer/lexer.h"
#include "node.h"
#include "utils/linked_list.h"

struct ast_fundec
{
    char *name;
    struct ast_node *fun; // shell_command
    struct linked_list *redirs;
};

struct ast_fundec *ast_parse_fundec(struct lexer *lexer);
int ast_eval_fundec(struct ast_fundec *cmd, struct linked_list *ptr,
                    __attribute((unused)) struct ast_eval_ctx *ctx);
void ast_free_fundec(struct ast_fundec *cmd);
void ast_print_fundec(struct ast_fundec *cmd);

#endif // !FUNDEC_H
