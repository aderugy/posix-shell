#ifndef HASH_MAP_H
#define HASH_MAP_H

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#include <stdbool.h>
#include <stddef.h>

struct pair_list
{
    char *key;
    void *value;
    struct pair_list *next;
};

struct hash_map
{
    struct pair_list **data;
    size_t size;
};

size_t hash(char *str);
struct hash_map *hash_map_init(size_t size);

void hash_map_insert(struct hash_map *hm, char *key, void *value);
void *hash_map_get(struct hash_map *hash_map, char *key);
bool hash_map_remove(struct hash_map *hash_map, char *key);

void hash_map_dump(struct hash_map *hash_map);
void hash_map_free(struct hash_map *hash_map);

#endif /* ! HASH_MAP_H */
