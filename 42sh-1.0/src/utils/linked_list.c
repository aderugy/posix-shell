#include "linked_list.h"

#include <err.h>
#include <stdlib.h>

struct linked_list *list_init(void)
{
    struct linked_list *list = calloc(1, sizeof(struct linked_list));
    if (!list)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    return list;
}

void list_free(struct linked_list *list, void (*free_fn)(void *))
{
    struct linked_list_element *head = list->head;
    while (head)
    {
        if (head->data && free_fn)
        {
            free_fn(head->data);
        }

        struct linked_list_element *next = head->next;
        free(head);
        head = next;
    }

    free(list);
}

void list_append(struct linked_list *list, void *data)
{
    struct linked_list_element *el =
        calloc(1, sizeof(struct linked_list_element));
    if (!el)
    {
        err(EXIT_FAILURE, "out of memory");
    }
    el->data = data;

    if (!list->head)
    {
        list->head = el;
        list->size++;
        return;
    }

    struct linked_list_element *head = list->head;
    while (head->next)
    {
        head = head->next;
    }

    head->next = el;
    list->size++;
}

void *list_get(struct linked_list *list, size_t index)
{
    if (index >= list->size)
    {
        errx(EXIT_FAILURE, "index out of bounds");
    }

    size_t i = 0;
    struct linked_list_element *head = list->head;
    for (; i < index; i++)
    {
        head = head->next;
    }

    return head->data;
}
