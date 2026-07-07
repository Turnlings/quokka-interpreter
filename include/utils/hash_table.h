#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stddef.h>
#include "token.h"
#include "utils/pair.h"

typedef struct HashTable {
    size_t size;
    Pair **buckets;
} HashTable;

HashTable *hashtable_create(size_t size);
void hashtable_set(HashTable *table, char *key, Value *value);
int hashtable_get(HashTable *table, const char *key, Value **out_value);
void hashtable_destroy(HashTable *table);
int unhandled_errors(HashTable *table, List *errors);

#endif