#include "hash_map.h"

#include <err.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// returns a 64bits hash_key for a null terminated string
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
size_t hash(char *key)
{
    size_t hash_key = FNV_OFFSET;
    for (char *p = key; *p; p++)
    {
        hash_key ^= (size_t)(unsigned char)(*p);
        hash_key *= FNV_PRIME;
    }

    return hash_key;
}

struct hash_map *hash_map_init(size_t size)
{
    struct hash_map *hm = calloc(1, sizeof(struct hash_map));
    if (!hm)
    {
        errx(EXIT_FAILURE, "hash_map_init: not enough memory");
    }

    hm->size = size;
    hm->data = calloc(size, sizeof(struct pair_list *));

    return hm;
}

void hash_map_insert(struct hash_map *hash_map, char *key, void *value)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        errx(EXIT_FAILURE, "hashmap not initialized");
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
        p = calloc(1, sizeof(struct pair_list));
        if (p == NULL)
        {
            errx(EXIT_FAILURE, "Not enough memory");
        }

        p->key = key;
        p->value = value;
        p->next = hash_map->data[index];
        hash_map->data[index] = p;
    }
    else
    {
        p->value = value;
    }
}

void pair_list_free(struct pair_list *pl)
{
    if (pl)
    {
        free(pl->key);
        free(pl->value);
        free(pl);
    }
}

// free the entire hashmap
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
                pair_list_free(p);
                p = hash_map->data[i];
            }

            free(hash_map->data[i]);
        }

        free(hash_map->data);
        free(hash_map);
    }
}
// prints the hashmap
void hash_map_dump(struct hash_map *hash_map)
{
    for (size_t i = 0; i < hash_map->size; i++)
    {
        struct pair_list *p = hash_map->data[i];
        if (p)
        {
            printf("%s: %p", p->key, p->value);
            p = p->next;

            while (p)
            {
                printf(", %s: %p", p->key, p->value);
                p = p->next;
            }

            printf("\n");
        }
    }
}

void *hash_map_get(struct hash_map *hash_map, char *key)
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
        perror("HASH_MAP: asking for non-existant parameter");
        return NULL;
    }

    return p->value;
}
bool hash_map_remove(struct hash_map *hash_map, char *key)
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

    struct pair_list *prev;
    struct pair_list *p = hash_map->data[index];
    if (p && strcmp(p->key, key) == 0)
    {
        hash_map->data[index] = p->next;
        free(p);
        return true;
    }

    while (p && strcmp(p->key, key))
    {
        prev = p;
        p = p->next;
    }

    if (p == NULL)
    {
        perror("HASH_MAP: removing non-existant parameter");
        return false;
    }

    prev->next = p->next;
    free(p);

    return true;
}
