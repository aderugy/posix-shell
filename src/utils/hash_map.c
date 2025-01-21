#define _POSIX_C_SOURCE 200809L
#include "hash_map.h"

#include <err.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast/nodes/node.h"
#include "mbtstr/str.h"
#include "utils/logger.h"
#include "utils/xalloc.h"

// returns a 64bits hash_key for a null terminated string
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static size_t hash(char *key)
{
    size_t hash_key = FNV_OFFSET;
    for (char *p = key; *p; p++)
    {
        hash_key ^= (size_t)(unsigned char)(*p);
        hash_key *= FNV_PRIME;
    }

    return hash_key;
}

static struct hash_map_elt *hash_map_elt_init(char *name, void *value,
                                              enum hash_map_elt_type type)
{
    struct hash_map_elt *p = xcalloc(1, sizeof(struct hash_map_elt));

    p->key = name;
    p->value = value;
    p->type = type;
    p->next = NULL;

    return p;
}

static void hash_map_elt_free(struct hash_map_elt *elt)
{
    if (elt)
    {
        free(elt->key);
        if (elt->type == HASH_FUNCTION)
        {
            ast_free(elt->value);
        }
        else
        {
            mbt_str_free(elt->value);
        }
        free(elt);
    }
}

struct hash_map *hash_map_init(size_t size)
{
    struct hash_map *hm = xcalloc(1, sizeof(struct hash_map));
    hm->size = size;
    hm->data = xcalloc(size, sizeof(struct hash_map_elt *));

    return hm;
}

// @refactor
static bool hash_map_remove(struct hash_map *hash_map, char *key,
                            enum hash_map_elt_type type)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        return false;
    }
    size_t index = hash(key);
    if (index >= hash_map->size)
    {
        index %= hash_map->size;
    }
    struct hash_map_elt *prev = NULL;
    struct hash_map_elt *p = hash_map->data[index];
    if (p && strcmp(p->key, key) == 0 && p->type == type)
    {
        hash_map->data[index] = p->next;
        hash_map_elt_free(p);
        return true;
    }
    while (p && strcmp(p->key, key) && p->type != type)
    {
        prev = p;
        p = p->next;
    }
    if (p == NULL)
    {
        return false;
    }
    prev->next = p->next;
    hash_map_elt_free(p);
    return true;
}

int hash_map_insert(struct hash_map *hash_map, char *name, void *value,
                    enum hash_map_elt_type type)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        warnx("hash map inset error");
        return 1;
    }

    // Removes the element if it exists
    hash_map_remove(hash_map, name, type);

    // AND index with capacity - 1 to ensure index < capacity
    size_t index = hash(name);
    index = (index & (hash_map->size - 1));

    // Loops through entries
    struct hash_map_elt *p = hash_map->data[index];
    while (p && strcmp(p->key, name) && p->type != type)
    {
        p = p->next;
    }

    // An empty slot was found
    if (p == NULL)
    {
        p = hash_map_elt_init(name, value, type);

        p->next = hash_map->data[index];
        hash_map->data[index] = p;
    }
    // Element already exists in the hash_map
    else
    {
        hash_map_elt_free(p->value);
        p->value = value;
    }
    return 0;
}

void hash_map_free(struct hash_map *hash_map)
{
    if (hash_map)
    {
        for (size_t i = 0; i < hash_map->size; i++)
        {
            struct hash_map_elt *p = hash_map->data[i];
            while (hash_map->data[i])
            {
                hash_map->data[i] = hash_map->data[i]->next;
                hash_map_elt_free(p);
                p = hash_map->data[i];
            }

            free(hash_map->data[i]);
        }

        free(hash_map->data);
        free(hash_map);
    }
}

void *hash_map_get(struct hash_map *hash_map, char *key,
                   enum hash_map_elt_type type)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        errx(EXIT_FAILURE, "hash map not initialized");
    }

    size_t index = hash(key);
    if (index >= hash_map->size)
    {
        index %= hash_map->size;
    }

    struct hash_map_elt *p = hash_map->data[index];
    while (p && strcmp(p->key, key) && p->type != type)
    {
        p = p->next;
    }

    if (p == NULL)
    {
        return NULL;
    }

    return p->value;
}
