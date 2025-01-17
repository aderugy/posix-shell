#define _POSIX_C_SOURCE 200809L

#include <err.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "builtins.h"
#include "commands.h"
#include "utils/logger.h"

#define MAX_PATH 4096

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

char *normalize_path(const char *path)
{
    // Buffer pour stocker le chemin absolu final
    static char resolved_path[MAX_PATH];
    char temp_path[MAX_PATH];
    char *token, *resolved;

    if (path[0] == '/')
    {
        resolved = resolved_path;
        resolved[0] = '/';
        resolved[1] = '\0';
    }
    else
    {
        if (!getcwd(resolved_path, sizeof(resolved_path)))
        {
            perror("getcwd");
            return NULL;
        }
        resolved = resolved_path + strlen(resolved_path);
    }

    snprintf(temp_path, sizeof(temp_path), "%s", path);
    temp_path[sizeof(temp_path) - 1] = '\0';

    token = strtok(temp_path, "/");
    while (token)
    {
        if (strcmp(token, ".") == 0)
        {
            token = strtok(NULL, "/");
            continue;
        }
        else if (strcmp(token, "..") == 0)
        {
            if (resolved > resolved_path + 1)
            {
                resolved--; // Ignore le dernier '/'
                while (resolved > resolved_path && resolved[-1] != '/')
                    resolved--;
            }
        }
        else
        {
            if (resolved > resolved_path + 1)
                *resolved++ = '/';
            strncpy(resolved, token, MAX_PATH - (resolved - resolved_path));
            resolved += strlen(token);
        }
        token = strtok(NULL, "/");
    }
    *resolved = '\0';
    return resolved_path;
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
        if (chdir(home) != 0)
            errx(1, "cd: chdir: error");
        return 0;
    }

    if (strcmp("-", argv[1]) == 0)
    {
        char *oldpwd = getenv("OLDPWD");
        char *current_path = get_current_path(); // RULE 2
        if (setenv("OLDPWD", current_path, 1) != 0)
            errx(1, "cd: setenv: error");
        if (setenv("PWD", oldpwd, 1) != 0)
            errx(1, "cd: setenv: error");
        if (chdir(oldpwd) != 0)
            errx(1, "cd: chdir: error");
        free(current_path);
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

    char *resolved_path = normalize_path(argv[1]);
    if (!resolved_path)
    {
        errx(1, "cd: Path error");
    }

    char *current_path = get_current_path(); // RULE 2
    if (setenv("OLDPWD", current_path, 1) != 0)
        errx(1, "cd: setenv: error");
    free(current_path);
    if (setenv("PWD", resolved_path, 1) != 0)
        errx(1, "cd: setenv: error");
    if (chdir(resolved_path) != 0)
        errx(1, "cd: chdir: error");

    logger("Finish cd\n");
    return 0;
}
