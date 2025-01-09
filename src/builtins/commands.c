#include "commands.h"

#include <err.h>

#include "run_command.h"

int register_commands(void)
{
    if (add_command(ECHO, echo))
    {
        errx(1, "%s: Couldn't register command", ECHO);
    }
    if (add_command(TRUE, true_builtin))
    {
        errx(1, "%s: Couldn't register command", TRUE);
    }
    if (add_command(FALSE, false_builtin))
    {
        errx(1, "%s: Couldn't register command", FALSE);
    }

    return 0;
}

int unregister_commands(void)
{
    if (del_command(ECHO))
    {
        errx(1, "%s: Couldn't delete command", ECHO);
    }
    if (del_command(TRUE))
    {
        errx(1, "%s: Couldn't delete command", TRUE);
    }
    if (del_command(FALSE))
    {
        errx(1, "%s: Couldn't delete command", FALSE);
    }

    return 0;
}
