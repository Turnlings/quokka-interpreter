#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#include "token.h"
#include "utils/pair.h"

typedef struct HashMap {
    size_t size;
    Pair **buckets;
} HashMap;

HashMap *hashmap_create(size_t size);
void hashmap_set(HashMap *hashmap, char *key, Value *value);
int hashmap_get(HashMap *hashmap, const char *key, Value **out_value);
void hashmap_destroy(HashMap *hashmap);

#endif