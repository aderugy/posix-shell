#ifndef SIMPLE_COMMAND_EXECUTE_NON_BUILTIN_H
#define SIMPLE_COMMAND_EXECUTE_NON_BUILTIN_H

#include <stdlib.h>

#include "command.h"
#include "lexer/lexer.h"
#include "node.h"
#include "simple_command.h"
#include "simple_command_execute_builtin.h"
#include "utils/linked_list.h"

int simple_command_execute_non_builtin(struct ast_simple_cmd *cmd, char **argv,
                                       struct ast_eval_ctx *ctx, int argc);

#endif // !SIMPLE_COMMAND_EXECUTE_NON_BUILTIN_H
