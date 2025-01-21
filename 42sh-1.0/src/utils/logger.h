#ifndef LOGGER_H
#define LOGGER_H

#include <stddef.h>

void logger(const char *s, ...);
void logger_buffer(const char *name, void *buf, size_t len);
void print_stack_trace(void);

#endif // !LOGGER_H
