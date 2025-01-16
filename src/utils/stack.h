#ifndef STACK_H
#define STACK_H

#include <stddef.h>

struct stack_elt
{
    void *data;
    struct stack_elt *next;
};

struct stack
{
    size_t size;
    struct stack_elt *head;
    void (*free_fn)(void *);
};

/*
 * pass free_fn if u want data to be freed
 */
struct stack *stack_init(void (*free_fn)(void *));
void stack_push(struct stack *stack, void *data);
void *stack_peek(struct stack *stack);
void *stack_pop(struct stack *stack);
void stack_free(struct stack *stack);

#endif // !STACK_H
