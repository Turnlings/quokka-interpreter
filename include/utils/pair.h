#ifndef PAIR_H
#define PAIR_H

#include "token.h"

typedef struct Pair {
    char *key;
    Value *value;
    struct Pair *next;
} Pair;

#endif