#include "commands.h"

#include <err.h>

#include "builtins.h"
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
    if (add_command(EXIT, exit_builtin))
    {
        errx(1, "%s: Couldn't register command", EXIT);
    }
    if (add_command(CD, cd))
    {
        errx(1, "%s: Couldn't register command", CD);
    }
    if (add_command(EXPORT, export_builtin))
    {
        errx(1, "%s: Couldn't register command", EXPORT);
    }
    if (add_command(UNSET, unset_builtin))
    {
        errx(1, "%s: Couldn't register command", UNSET);
    }
    if (add_command(DOT, dot))
    {
        errx(1, "%s: Couldn't register command", DOT);
    }
    if (add_command(BREAK_BUILTIN, break_builtin))
    {
        errx(1, "%s: Couldn't register command", BREAK_BUILTIN);
    }
    if (add_command(CONTINUE_BUILTIN, continue_builtin))
    {
        errx(1, "%s: Couldn't register command", CONTINUE_BUILTIN);
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
    if (del_command(EXIT))
    {
        errx(1, "%s: Couldn't delete command", EXIT);
    }
    if (del_command(CD))
    {
        errx(1, "%s: Couldn't delete command", CD);
    }
    if (del_command(EXPORT))
    {
        errx(1, "%s: Couldn't delete command", EXPORT);
    }
    if (del_command(UNSET))
    {
        errx(1, "%s: Couldn't delete command", UNSET);
    }
    if (del_command(DOT))
    {
        errx(1, "%s: Couldn't delete command", DOT);
    }
    if (del_command(BREAK_BUILTIN))
    {
        errx(1, "%s: Couldn't delete command", BREAK_BUILTIN);
    }
    if (del_command(CONTINUE_BUILTIN))
    {
        errx(1, "%s: Couldn't delete command", CONTINUE_BUILTIN);
    }

    return 0;
}
