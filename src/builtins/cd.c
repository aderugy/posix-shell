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
#include "utils/my_strcat.h"
#include "utils/xalloc.h"

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
        buf = xrealloc(buf, size);
        ptr = getcwd(buf, size);
        if (ptr == NULL)
        {
            warnx("cd: ptr NULL"); // 1
            return NULL;
        }
    }
    return buf;
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
            warnx("cd: normalize_path: not a good path");
            return NULL;
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

    *resolved = 0;
    return resolved_path;
}

int move_cd(char *oldpwd, char *pwd)
{
    if (setenv("OLDPWD", oldpwd, 1) != 0)
    {
        warnx("cd: setenv: error");
        return 1;
    }
    if (setenv("PWD", pwd, 1) != 0)
    {
        warnx("cd: setenv: error");
        return 1;
    }
    if (chdir(pwd) != 0)
    {
        warnx("cd: chdir: error");
        return 1;
    }
    return 0;
}

char *my_reverse_strcat(char *src, char *dest)
{
    size_t total_len = strlen(src) + strlen(dest);
    char *result = xmalloc(total_len + 1);
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

static int tiret(void)
{
    char *oldpwd = getenv("OLDPWD");
    char *current_path = get_current_path(); // RULE 2
    if (!current_path)
    {
        return 2;
    }
    int verification = move_cd(current_path, oldpwd);
    if (verification == 0)
        printf("%s\n", oldpwd);
    free(current_path);
    return verification;
}

static int cd_with_no_arg(void)
{
    char *home = getenv("HOME");
    if (home == NULL || strlen(home) == 0) // RULE 1
    {
        warnx("cd: empty or unddefined HOME environment");
        return 2;
    }
    char *current_path = get_current_path(); // RULE 2
    if (!current_path)
        return 1;
    int verification = move_cd(current_path, home);
    free(current_path);
    return verification;
}

int cd(int argc, char **argv,
       __attribute__((unused)) struct ast_eval_ctx *ast_eval_ctx)
{
    if (argc > 2)
    {
        warnx("cd: too many arguments");
        return 2;
    }

    if (argc == 1)
    {
        return cd_with_no_arg();
    }

    if (strcmp("-", argv[1]) == 0)
    {
        return tiret();
    }

    struct stat path_stat;
    if (stat(argv[1], &path_stat) != 0)
    {
        warnx("cd: No such file or directory");
        return 1;
    }

    if (!S_ISDIR(path_stat.st_mode))
    {
        warnx("cd: Not a directory");
        return 1;
    }

    char *resolved_path = normalize_path(argv[1]);
    if (!resolved_path)
        return 2;

    char *current_path = get_current_path(); // RULE 2
    if (!current_path)
        return 1;
    logger("current_path: %s !", resolved_path);
    int verification;
    if (argv[1][0] == '/')
    {
        resolved_path = my_reverse_strcat(resolved_path, "/");
        verification = move_cd(current_path, resolved_path);
        free(resolved_path);
    }
    else
    {
        size_t len = strlen(resolved_path) - 1;
        if (resolved_path[len] == '/')
        {
            resolved_path[len] = 0;
        }
        verification = move_cd(current_path, resolved_path);
    }
    free(current_path);
    return verification;
}
