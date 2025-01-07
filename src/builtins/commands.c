#include "commands.h"

#include <err.h>

#include "run_command.h"

int register_commands(void)
{
    if (add_command(ECHO, echo))
    {
        errx(1, "%s: Couldn't register command", ECHO);
    }
    if (add_command(TRUE_BUILTIN, true_builtin))
    {
        errx(1, "%s: Couldn't register command", TRUE_BUILTIN);
    }
    if (add_command(FALSE_BUILTIN, false_builtin))
    {
        errx(1, "%s: Couldn't register command", FALSE_BUILTIN);
    }

    return 0;
}

int unregister_commands(void)
{
    if (del_command(ECHO))
    {
        errx(1, "%s: Couldn't delete command", ECHO);
    }
    if (del_command(TRUE_BUILTIN))
    {
        errx(1, "%s: Couldn't delete command", TRUE_BUILTIN);
    }
    if (del_command(FALSE_BUILTIN))
    {
        errx(1, "%s: Couldn't delete command", FALSE_BUILTIN);
    }

    return 0;
}
