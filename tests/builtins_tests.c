#include "builtins/commands.h"

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[])
{
    register_commands();
    unregister_commands();

    return 0;
}
