#include <stdio.h>
#include "features/list.h"

List *list_create(int length) {
    List *list = malloc(sizeof(List));
    list->items = malloc(length*sizeof(Value));
    list->array_length = length;
    list->tail = -1;

    return list;
}

void list_copy(List *original, List *target) {
    if (original->tail > target->array_length) {
        fprintf(stderr, "Cannot copy list to smaller list\n");
        return;
    }

    for (int i = 0; i <= original->tail; i++) {
        target->items[i] = original->items[i];
    }

    target->tail = original->tail;
}

void list_add(List **plist, Value *item) {
    List *list = *plist;

    if (list->tail + 1 >= list->array_length) {
        List *new_list = list_create(list->array_length*2);
        list_copy(list, new_list);
        list_destroy(list); // Cleanup old list
        list = new_list;
        *plist = new_list;
    }

    list->items[++list->tail] = item;
}

Value *list_access(List *list, int index) {
    if (index < 0 || index > list->tail) {
        fprintf(stderr, "Invalid index for list\n");
        return NULL;
    }

    return list->items[index];
}

void list_destroy(List *list) {
    free(list->items);
    free(list);
}