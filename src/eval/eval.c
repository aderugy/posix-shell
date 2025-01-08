#include "eval/eval.h"

#include "builtins/commands.h"
#include "builtins/run_command.h"
#include "utils/logger.h"

int eval(__attribute__((unused)) struct ast_node *ast_node)
{
    register_commands(); // load builtins

    unregister_commands(); // unload builtins
    return 0;
}
