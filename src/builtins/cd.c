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

static char *my_strcat(char *src, char *dest)
{
    size_t total_len = strlen(src) + strlen(dest);
    char *result = malloc(total_len + 1);
    size_t i = 0;
    size_t j = 0;
    while (dest[j] != 0)
    {
        result[i] = dest[j];
        j++;
        i++;
    }
    j = 0;
    while (src[j] != 0)
    {
        result[i] = src[j];
        j++;
        i++;
    }
    result[i] = 0;
    return result;
}

char *normalize_path(const char *path)
{
    static char resolved_path[MAX_PATH];
    char temp_path[MAX_PATH];
    char *token;
    char *resolved;

    if (path[0] == '/') // On verifie si le chemin commence à la racine sinon on
                        // le rajoute
    {
        resolved = resolved_path;
        resolved[0] = '/';
        resolved[1] = 0;
    }
    else
    {
        if (!getcwd(resolved_path, sizeof(resolved_path)))
        {
            errx(2, "cd: normalize_path: not a good path");
        }
        resolved = resolved_path + strlen(resolved_path);
    }
    logger("resolved: %s ?\n", resolved);

    snprintf(temp_path, sizeof(temp_path), "%s", path);
    temp_path[sizeof(temp_path) - 1] = '\0';

    token = strtok(temp_path, "/");
    while (token)
    {
        if (strcmp(token, ".") == 0) // s'il y a seulement . on ignore
        {
            token = strtok(NULL, "/");
            continue;
        }
        else if (strcmp(token, "..") == 0) // On met le chemin d'avant
        {
            if (resolved > resolved_path + 1)
            {
                resolved--;
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

void move_cd(char *oldpwd, char *pwd)
{
    if (setenv("OLDPWD", oldpwd, 1) != 0)
        errx(1, "cd: setenv: error");
    if (setenv("PWD", pwd, 1) != 0)
        errx(1, "cd: setenv: error");
    if (chdir(pwd) != 0)
        errx(1, "cd: chdir: error");
}

int cd(int argc, char **argv,
       __attribute__((unused)) struct ast_eval_ctx *ast_eval_ctx)
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
        move_cd(current_path, home);
        free(current_path);
        return 0;
    }

    if (strcmp("-", argv[1]) == 0)
    {
        char *oldpwd = getenv("OLDPWD");
        char *current_path = get_current_path(); // RULE 2
        move_cd(current_path, oldpwd);
        printf("%s\n", oldpwd);
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

    char *current_path = get_current_path(); // RULE 2
    logger("current_path: %s !", resolved_path);
    if (argv[1][0] == '/')
    {
        resolved_path = my_strcat(resolved_path, "/");
        move_cd(current_path, resolved_path);
        free(resolved_path);
    }
    else
        move_cd(current_path, resolved_path);
    free(current_path);

    return 0;
}
