#ifndef RUN_COMMAND_H
#define RUN_COMMAND_H

#include <stddef.h>

struct runnable
{
    const char *name;
    int (*command)(int, char **);
};

struct runnables_list
{
    size_t size;
    size_t capacity;
    struct runnable **list;
};

int add_command(const char *name, int (*command)(int, char **));
int del_command(const char *name);
int run_command(int argc, char **argv);

struct runnable *get_command(const char *name, size_t *index);
#endif // !RUN_COMMAND_H
