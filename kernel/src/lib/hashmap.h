#pragma once

#include <stddef.h>

typedef struct {
    const char *key;
    void *value;
} hashmap_entry_t;

typedef struct {
    hashmap_entry_t *table;
    size_t capacity;
    size_t size;
} hashmap_t;

int hashmap_init(hashmap_t *map, size_t initial_capacity);
int hashmap_put(hashmap_t *map, const char *key, void *value);
void *hashmap_get(hashmap_t *map, const char *key);