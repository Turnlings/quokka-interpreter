#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "token.h"

typedef struct Pair {
    char *key;
    Value value;
    struct Pair *next;
} Pair;

typedef struct HashTable {
    size_t size;
    Pair **buckets;
} HashTable;

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
 * @brief Create a hashtable with a given number of buckets.
 * @param size The number of buckets in the hash table.
 * @return A pointer to the hash table.
 */
HashTable *hashtable_create(size_t size) {
    HashTable* table = malloc(sizeof(HashTable));
    if (!table) return NULL;
    table->size = size;
    table->buckets = malloc(size * sizeof(Pair*));
    if (!table->buckets) {
        free(table);
        return NULL;
    }
    return table;
}

/**
 * @brief Set a key value pair of a hash table.
 * @param table A pointer to the hash table to add to.
 * @param key The key string.
 * @param value The value associated with the key.
 */
void hashtable_set(HashTable *table, char *key, Value *value) {
    if (value == NULL) {
        printf("Attempted to assign null to key: %s\n", key);
    }
    unsigned int pos = hash(key, table->size);
    Pair *entry = table->buckets[pos];

    while (entry) {
        // Identical key: update value
        if (strcmp(entry->key, key) == 0) {
            entry->value = *value;
            return;
        }
        // Collision: append entry to linked list
        entry = entry->next;
    }

    // No collision: add entry at that position
    Pair *new_entry = malloc(sizeof(Pair));
    new_entry->key = strdup(key);
    new_entry->value = *value;
    new_entry->next = table->buckets[pos];
    table->buckets[pos] = new_entry;
}

/**
 * @brief Get the value of the hashtable at the key and a status code.
 *        "Proper" return value is the out_value.   
 * @param table A pointer to the hash table to get from.
 * @param key The key from which to get the value.
 * @param out_value A pointer to the value at the key.
 * @return Status of 1 if successful and 0 if not.
 */
int hashtable_get(HashTable *table, const char *key, Value **out_value) {
    if (!table) return 0;
    unsigned int pos = hash(key, table->size);
    Pair *entry = table->buckets[pos];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            *out_value = &entry->value;
            return 1;
        }
        entry = entry->next;
    }
    return 0;
}

/**
 * @brief Properly handles the deletion of all parts of the hash table.
 * @param table The hash table to destroy.
 */
void hashtable_destroy(HashTable *table) {
    if (!table) return;
    for (size_t i = 0; i < table->size; i++) {
        Pair *entry = table->buckets[i];
        while (entry) {
            Pair *next = entry->next;
            free(entry->key);
            value_destroy(entry->value);
            free(entry);
            entry = next;
        }
    }
    free(table->buckets);
    free(table);
}