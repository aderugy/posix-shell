#define _POSIX_C_SOURCE 200809L
#include "hash_map.h"

#include <err.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static void hash_map_pair_list_free(struct pair_list *pl)
{
    if (pl)
    {
        free(pl->key);
        mbt_str_free(pl->value);
        free(pl);
    }
}

struct hash_map *hash_map_init(size_t size)
{
    struct hash_map *hm = xcalloc(1, sizeof(struct hash_map));
    hm->size = size;
    hm->data = xcalloc(size, sizeof(struct pair_list *));

    return hm;
}

// @REFACTOR
// Pourquoi key et pas name comme les fonctions qui appelle celle ci et pourquoi
// refaire un dup ?
int hash_map_insert(struct hash_map *hash_map, char *key, char *value)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        warnx("hash map inset error");
        return 1;
    }

    // AND index with capacity - 1 to ensure index < capacity
    size_t index = hash(key);
    index = (index & (hash_map->size - 1));

    // Loops through entries
    struct pair_list *p = hash_map->data[index];
    while (p && strcmp(p->key, key))
    {
        p = p->next;
    }

    // An empty slot was found
    if (p == NULL)
    {
        p = xcalloc(1, sizeof(struct pair_list));

        p->key = strdup(key); // Pourquoi strdup ?
        p->value = mbt_str_init(42);
        p->next = hash_map->data[index];
        hash_map->data[index] = p;
    }
    else
    {
        mbt_str_dtor(p->value);
    }

    mbt_str_pushcstr(p->value, value);
    return 0;
}

void hash_map_free(struct hash_map *hash_map)
{
    if (hash_map)
    {
        for (size_t i = 0; i < hash_map->size; i++)
        {
            struct pair_list *p = hash_map->data[i];
            while (hash_map->data[i])
            {
                hash_map->data[i] = hash_map->data[i]->next;
                hash_map_pair_list_free(p);
                p = hash_map->data[i];
            }

            free(hash_map->data[i]);
        }

        free(hash_map->data);
        free(hash_map);
    }
}
// prints the hashmap

struct mbt_str *hash_map_get(struct hash_map *hash_map, char *key)
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

    struct pair_list *p = hash_map->data[index];
    while (p && strcmp(p->key, key))
    {
        p = p->next;
    }

    if (p == NULL)
    {
        return NULL;
    }

    return p->value;
}
