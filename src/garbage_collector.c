#include "token.h"

void gc_reference(Value *value) {
    value->references = value->references + 1;
}

void gc_dereference(Value *value) { 
    value->references = value->references - 1;

    if (value->references == 0) {
        value_destroy(*value);
        free(value);
    }
}

Value *gc_malloc() {
    Value *value = calloc(1, sizeof(Value));
    if (!value) { return NULL; }
    value->references = 0;
    return value;
}