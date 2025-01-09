#include "run_command.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct runnables_list *commands = NULL;

int last_status = 0;

static int init_commands(void)
{
    commands = calloc(1, sizeof(struct runnables_list));

    if (!commands)
    {
        return 1;
    }

    commands->capacity = 8;
    commands->list = calloc(8, sizeof(struct runnable *));

    if (!commands->list)
    {
        return 1;
    }

    return 0;
}

static struct runnable *get_command(const char *name, size_t *index)
{
    if (!commands || !name)
    {
        return NULL;
    }

    size_t i = 0;
    for (; i < commands->size; i++)
    {
        if (strcmp(name, commands->list[i]->name) == 0)
        {
            if (index)
            {
                *index = i;
            }

            return commands->list[i];
        }
    }

    return NULL;
}

static void free_commands(void)
{
    if (!commands)
    {
        return;
    }

    for (size_t i = 0; i < commands->size; i++)
    {
        free(commands->list[i]);
    }

    free(commands->list);
    free(commands);
    commands = NULL;
}

int add_command(const char *name, int (*command)(int, char **))
{
    if (!name || !command || (!commands && init_commands() == 1))
    {
        return -1;
    }

    if (get_command(name, NULL))
    {
        return -1;
    }

    struct runnable *cmd = calloc(1, sizeof(struct runnable));
    if (!cmd)
    {
        return -1;
    }
    cmd->name = name;
    cmd->command = command;

    if (commands->size == commands->capacity)
    {
        struct runnable **tmp = realloc(commands->list, 2 * commands->capacity);

        if (!tmp)
        {
            free(cmd);
            return -1;
        }

        commands->list = tmp;
        commands->capacity *= 2;
    }

    commands->list[commands->size] = cmd;
    commands->size++;

    return 0;
}

int del_command(const char *name)
{
    if (!commands || !name)
    {
        return -1;
    }

    size_t i = 0;

    struct runnable *cmd = get_command(name, &i);
    if (!cmd)
    {
        return -1;
    }

    free(cmd);

    for (; i + 1 < commands->size; i++)
    {
        commands->list[i] = commands->list[i + 1];
    }

    commands->size--;

    if (commands->size == 0)
    {
        free_commands();
    }

    return 0;
}

int run_command(int argc, char **argv)
{
    struct runnable *cmd;
    if (argc == 0 || !argv || (cmd = get_command(argv[0], NULL)) == NULL)
    {
        return 127;
    }

    return cmd->command(argc, argv);
}
