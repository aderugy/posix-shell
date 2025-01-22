#ifndef HASH_MAP_H
#define HASH_MAP_H

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#include <stdbool.h>
#include <stddef.h>

#include "mbtstr/str.h"

enum hash_map_elt_type
{
    HASH_VARIABLE, // char *
    HASH_FUNCTION // ast_node *
};

struct hash_map_elt
{
    char *key;
    enum hash_map_elt_type type;
    void *value;
    struct hash_map_elt *next;
};

struct hash_map
{
    struct hash_map_elt **data;
    size_t size;
};

struct hash_map *hash_map_init(size_t size);

int hash_map_insert(struct hash_map *hash_map, char *name, void *value,
                    enum hash_map_elt_type type);

void *hash_map_get(struct hash_map *hash_map, char *key,
                   enum hash_map_elt_type type);

void hash_map_free(struct hash_map *hash_map);
bool hash_map_remove(struct hash_map *hash_map, char *key,
                     enum hash_map_elt_type type);

#endif /* ! HASH_MAP_H */
