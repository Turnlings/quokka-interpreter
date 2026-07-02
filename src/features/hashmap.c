#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "token.h"
#include "garbage_collector.h"
#include "features/hashmap.h"

/**
 * @brief djb2 hash function.
 * @param key The value to be hashed.
 * @param size The max size the return value can be.
 * @return The hashed key.
 */
static unsigned int hash(const char* key, size_t size) {
    unsigned int hash = 5381;
    int c;

    while ((c = *key++))
        hash = ((hash << 5) + hash) + c;

    return hash % size;
}

/**
 * @brief Create a hashmap with a given number of buckets.
 * @param size The number of buckets in the hashmap.
 * @return A pointer to the hash hashmap.
 */
HashMap *hashmap_create(size_t size) {
    HashMap* hashmap = malloc(sizeof(HashMap));
    if (!hashmap) return NULL;
    hashmap->size = size;
    hashmap->buckets = calloc(size, sizeof(Pair*));
    if (!hashmap->buckets) {
        free(hashmap);
        return NULL;
    }
    return hashmap;
}

/**
 * @brief Set a key value pair of a hashmap.
 * @param hashmap A pointer to the hashmap to add to.
 * @param key The key string.
 * @param value The value associated with the key.
 */
void hashmap_set(HashMap *hashmap, char *key, Value *value) {
    if (value == NULL) {
        printf("Attempted to assign null to key: %s\n", key);
    }

    gc_reference(value);

    unsigned int pos = hash(key, hashmap->size);
    Pair *entry = hashmap->buckets[pos];

    while (entry) {
        // Identical key: update value
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;
            return;
        }
        // Collision: append entry to linked list
        entry = entry->next;
    }

    // No collision: add entry at that position
    Pair *new_entry = malloc(sizeof(Pair));
    new_entry->key = strdup(key);
    new_entry->value = value;
    new_entry->next = hashmap->buckets[pos];
    hashmap->buckets[pos] = new_entry;
}

/**
 * @brief Get the value of the hashmap at the key and a status code.
 *        "Proper" return value is the out_value.   
 * @param hashmap A pointer to the hashmap to get from.
 * @param key The key from which to get the value.
 * @param out_value A pointer to the value at the key.
 * @return Status of 1 if successful and 0 if not.
 */
int hashmap_get(HashMap *hashmap, const char *key, Value **out_value) {
    if (!hashmap) return 0;
    unsigned int pos = hash(key, hashmap->size);
    Pair *entry = hashmap->buckets[pos];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            *out_value = entry->value;
            return 1;
        }
        entry = entry->next;
    }
    return 0;
}

/**
 * @brief Properly handles the deletion of all parts of the hashmap.
 * @param hashmap The hashmap to destroy.
 */
void hashmap_destroy(HashMap *hashmap) {
    if (!hashmap) return;
    for (size_t i = 0; i < hashmap->size; i++) {
        Pair *entry = hashmap->buckets[i];
        while (entry) {
            Pair *next = entry->next;
            free(entry->key);
            gc_dereference(entry->value);
            free(entry);
            entry = next;
        }
    }
    free(hashmap->buckets);
    free(hashmap);
}