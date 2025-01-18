#ifndef SIMPLE_COMMAND_H
#define SIMPLE_COMMAND_H

#include "ast_simple_cmd.h"
#include "lexer/lexer.h"
#include "node.h"
#include "simple_command_execute_builtin.h"
#include "simple_command_execute_non_builtin.h"
#include "utils/linked_list.h"

struct ast_simple_cmd *ast_parse_simple_cmd(struct lexer *lexer);
int ast_eval_simple_cmd(struct ast_simple_cmd *cmd, void **out,
                        __attribute((unused)) struct ast_eval_ctx *ctx);
void ast_free_simple_cmd(struct ast_simple_cmd *cmd);

void ast_print_simple_cmd(struct ast_simple_cmd *cmd);

#endif // !SIMPLE_COMMAND_H
