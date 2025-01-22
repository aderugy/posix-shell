#include "logger.h"

#include <execinfo.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int logger_stack_idx = 0;

void logger(const char *s, ...)
{
    static bool log = false;

#ifdef LOG_BLAME
    void *buffer[10];
    char **symbols;
    int num_frames;

    // Get the backtrace
    num_frames = backtrace(buffer, 10);

    // Get function names
    symbols = backtrace_symbols(buffer, num_frames);

    if (symbols == NULL)
    {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    // Print the function above the current one
    if (num_frames > 2)
    {
        printf("Function above current: %s\n", symbols[2]);
    }
    else
    {
        printf("No function above the current one.\n");
    }

    free(symbols);
#endif /* ifdef LOG_BLAME */

    if (s == NULL)
    {
        log = true;
    }
    else if (log)
    {
        for (int i = 0; i < logger_stack_idx - 1; i++)
        {
            printf("  ");
        }
        if (logger_stack_idx)
        {
            printf("-> ");
        }

        va_list ap;
        va_start(ap, s);
        vprintf(s, ap);
        va_end(ap);
    }
}

void logger_buffer(const char *name, void *buf, size_t len)
{
    unsigned char *c_buf = buf;

    printf("%s (%zu bytes): ", name, len);
    for (size_t i = 0; i < len; i++)
    {
        printf("%02X.", c_buf[i]);
    }
    printf("\n");
}

void print_stack_trace(void)
{
    void *buffer[100]; // Buffer to store backtrace
    int size;

    // Capture the backtrace
    size = backtrace(buffer, 100);

    // Print stack trace to stderr
    fprintf(stderr, "Stack trace (most recent call first):\n");
    backtrace_symbols_fd(buffer, size, STDERR_FILENO);
}
