#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>

struct linked_list_element
{
    void *data;
    struct linked_list_element *next;
};

struct linked_list
{
    size_t size;
    struct linked_list_element *head;
};

struct linked_list *list_init(void);
void list_free(struct linked_list *list, void (*free_fn)(void *));

void list_append(struct linked_list *list, void *data);
void *list_get(struct linked_list *list, size_t index);

#endif // !LINKED_LIST_H
