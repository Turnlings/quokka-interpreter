#ifndef LIST_H
#define LIST_H

#include "token.h"
#include <stdio.h>

typedef struct List {
    Value **items;
    int array_length;
    int tail;
} List;

List *list_create(int length);
void list_copy(List *original, List *target, int offset);
void list_add(List **plist, Value *item);
Value *list_access(List *list, int index);
void list_destroy(List *list);

#endif