#include "stack.h"

#include <err.h>
#include <stdlib.h>

#include "err_utils.h"

struct stack *stack_init(void (*free_fn)(void *))
{
    struct stack *stack = calloc(1, sizeof(struct stack));
    CHECK_MEMORY_ERROR(stack);

    stack->free_fn = free_fn;
    return stack;
}

void stack_push(struct stack *stack, void *data)
{
    struct stack_elt *elt = calloc(1, sizeof(struct stack_elt));
    CHECK_MEMORY_ERROR(elt);

    elt->data = data;
    elt->next = stack->head;
    stack->head = elt;
    stack->size++;
}

void *stack_peek(struct stack *stack)
{
    if (!stack->head)
    {
        return NULL;
    }

    return stack->head->data;
}

void *stack_pop(struct stack *stack)
{
    if (!stack->head)
    {
        return NULL;
    }

    struct stack_elt *head = stack->head;
    stack->head = head->next;
    stack->size--;

    void *data = head->data;
    free(head);

    return data;
}

void stack_free(struct stack *stack)
{
    struct stack_elt *head = stack->head;
    while (head)
    {
        struct stack_elt *next = head->next;
        if (head->data && stack->free_fn)
        {
            stack->free_fn(head->data);
        }

        free(head);
        head = next;
    }

    free(stack);
}
