#ifndef STR_H
#define STR_H

////////////////////////////////////////////////////////////////////////////////
/// THIS FILE WILL BE OVERWRITTEN                                            ///
////////////////////////////////////////////////////////////////////////////////

// mbtutils
#include "utils.h"
// libc
#include <stdbool.h>
#include <stddef.h>

struct mbt_str
{
    char *data;
    size_t size;
    size_t capacity;
};

bool mbt_str_ctor(struct mbt_str *str, size_t capacity) MBT_NONNULL(1);
void mbt_str_dtor(struct mbt_str *str);

struct mbt_str *mbt_str_init(size_t capacity);
void mbt_str_free(struct mbt_str *str);

bool mbt_str_pushc(struct mbt_str *str, char c) MBT_NONNULL(1);
bool mbt_str_pushcstr(struct mbt_str *str, const char *cstr) MBT_NONNULL(1);
void mbt_str_merge(struct mbt_str *str1, struct mbt_str *str2);

char mbt_str_pop(struct mbt_str *str);
void mbt_str_fill(struct mbt_str *str, struct mbt_str *to_fill, char c);

#endif /* !STR_H */
