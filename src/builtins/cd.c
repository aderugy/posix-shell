#define _POSIX_C_SOURCE 200809L

#include <err.h>
#include <getopt.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "builtins.h"
#include "commands.h"

char *get_current_path(void)
{
    long path_max;
    size_t size;
    char *buf = NULL;
    char *ptr = NULL;
    path_max = pathconf(".", _PC_PATH_MAX);
    if (path_max == -1)
        size = 1024;
    else if (path_max > 10240)
        size = 10240;
    else
        size = path_max;

    for (; ptr == NULL; size *= 2)
    {
        if ((buf = realloc(buf, size)) == NULL)
        {
            errx(1, "cd: out of memory");
        }

        ptr = getcwd(buf, size);
        if (ptr == NULL)
        {
            errx(1, "cd: ptr NULL");
        }
    }
    return buf;
}

int cd(int argc, char **argv)
{
    if (argc > 2)
    {
        errx(2, "cd: too many arguments");
    }

    if (argc == 1)
    {
        char *home = getenv("HOME");
        if (home == NULL || strlen(home) == 0) // RULE 1
            errx(2, "cd: empty or unddefined HOME environment");
        char *current_path = get_current_path(); // RULE 2
        setenv("OLDPWD", current_path, 1);
        free(current_path);
        setenv("PWD", home, 1);
        return 0;
    }

    if (strcmp("-", argv[1]) == 0)
    {
        char *oldpwd = getenv("OLDPWD");
        char *current_path = get_current_path(); // RULE 2
        if (setenv("OLDPWD", current_path, 1))
            errx(1, "cd: setenv: error");
        setenv("PWD", oldpwd, 1);
        return 0;
    }

    struct stat path_stat;
    if (stat(argv[1], &path_stat) != 0)
    {
        errx(1, "cd: No such file or directory");
    }

    if (!S_ISDIR(path_stat.st_mode))
    {
        errx(1, "cd: Not a directory");
    }

    char *resolved_path = realpath(argv[1], NULL);
    if (!resolved_path)
    {
        errx(1, "cd: Path error");
    }

    char *current_path = get_current_path(); // RULE 2
    setenv("OLDPWD", current_path, 1);
    free(current_path);
    setenv("PWD", resolved_path, 1);
    free(resolved_path);
    return 0;
}
