#include "mm/paging.h"
#include <stdint.h>
#include <stddef.h>
#include <mm/vma.h>
#include <lib/hashmap.h>

static uint32_t hash_path(const char *str) {
    uint32_t hash = 5381;
    char c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

int hashmap_init(hashmap_t *map, size_t initial_capacity) {
    void *mem = NULL;
    if ((mem = vma_alloc(vma_kernel_ctx, initial_capacity * sizeof(hashmap_entry_t), PTE_PRESENT | PTE_WRITE)) < 0)
        return -1;

    map->table = (hashmap_entry_t *)mem;
    map->capacity = initial_capacity;
    map->size = 0;

    for (size_t i = 0; i < map->capacity; i++) {
        map->table[i].key = NULL;
        map->table[i].value = NULL;
    }

    return 0;
}

static int hashmap_resize(hashmap_t *map) {
    size_t new_capacity = map->capacity * 2;
    void *new_mem = NULL;
    if ((new_mem = vma_alloc(vma_kernel_ctx, new_capacity * sizeof(hashmap_entry_t), PTE_PRESENT | PTE_WRITE)) < 0)
        return -1;

    hashmap_entry_t *new_table = (hashmap_entry_t *)new_mem;

    for (size_t i = 0; i < new_capacity; i++) {
        new_table[i].key = NULL;
        new_table[i].value = NULL;
    }

    for (size_t i = 0; i < map->capacity; i++) {
        if (map->table[i].key) {
            const char *key = map->table[i].key;
            void *val = map->table[i].value;
            uint32_t h = hash_path(key);
            for (size_t j = 0; j < new_capacity; j++) {
                size_t idx = (h + j) % new_capacity;
                if (!new_table[idx].key) {
                    new_table[idx].key = key;
                    new_table[idx].value = val;
                    break;
                }
            }
        }
    }

    vma_free(vma_kernel_ctx, map->table);

    map->table = new_table;
    map->capacity = new_capacity;

    return 0;
}

int hashmap_put(hashmap_t *map, const char *key, void *value) {
    if (map->size >= (map->capacity * 3 / 4)) {
        if (hashmap_resize(map) < 0)
            return -1;
    }

    uint32_t h = hash_path(key);
    for (size_t i = 0; i < map->capacity; i++) {
        size_t idx = (h + i) % map->capacity;

        if (!map->table[idx].key) {
            map->table[idx].key = key;
            map->table[idx].value = value;
            map->size++;
            return 0;
        }

        const char *k = map->table[idx].key;
        const char *p1 = k, *p2 = key;
        while (*p1 && *p2 && *p1 == *p2) { p1++; p2++; }
        if (*p1 == '\0' && *p2 == '\0') {
            map->table[idx].value = value;
            return 0;
        }
    }

    return -1;
}

void *hashmap_get(hashmap_t *map, const char *key) {
    uint32_t h = hash_path(key);
    for (size_t i = 0; i < map->capacity; i++) {
        size_t idx = (h + i) % map->capacity;

        if (!map->table[idx].key)
            return NULL;

        const char *k = map->table[idx].key;
        const char *p1 = k, *p2 = key;
        while (*p1 && *p2 && *p1 == *p2) {
            p1++; p2++;
        }
        if (*p1 == '\0' && *p2 == '\0')
            return map->table[idx].value;
    }
    return NULL;
}
