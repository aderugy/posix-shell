#ifndef COMMANDS_H
#define COMMANDS_H

#include "builtins.h"

// @Remark; functions were moved to avoid the following clang-tidy:
// 'file.h containg ten or more function declarations'
#define ECHO "echo"
#define TRUE "true"
#define FALSE "false"
#define EXIT "exit"
#define CD "cd"
#define EXPORT "export"
#define UNSET "unset"
#define DOT "."
#define BREAK_BUILTIN "break"
#define CONTINUE_BUILTIN "continue"

int register_commands(void);
int unregister_commands(void);

#endif // !COMMANDS_H
