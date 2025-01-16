#ifndef ERR_UTILS_H
#define ERR_UTILS_H

#include <err.h>
#include <stdlib.h>

#define CHECK_MEMORY_ERROR(ptr)                                                \
    if (!ptr)                                                                  \
    {                                                                          \
        errx(EXIT_FAILURE, "out of memory");                                   \
    }

#endif // !ERR_UTILS_H
