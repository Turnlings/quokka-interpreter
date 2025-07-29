#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stddef.h>

typedef struct Pair {
    char *key;
    double value;
    struct Pair *next;
} Pair;

typedef struct HashTable {
    size_t size;
    Pair **buckets;
} HashTable;

HashTable *hash_table_create(size_t size);
void hash_table_set(HashTable *table, char *key, double value);
int hashtable_get(HashTable *table, const char *key, double *out_value);
void hashtable_destroy(HashTable *table);

#endif