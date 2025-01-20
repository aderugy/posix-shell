#ifndef COMMANDS_H
#define COMMANDS_H

#include "builtins.h"

// @Remark; functions were moved to avoid the following clang-tidy:
// 'file.h containg ten or more function declarations'

int register_commands(void);
int unregister_commands(void);

#endif // !COMMANDS_H
