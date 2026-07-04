#include <string.h>
#include "token.h"
#include "features/list.h"

typedef enum {
    STD_LEN,
    STD_RANGE
} StdLib;

Value *std_len(Value *arg) {
    if (arg->type == TYPE_STRING) {
        Value *value = value_create(TYPE_INT);
        value->data.intValue = strlen(arg->data.stringValue);
        return value;
    }
    else if (arg->type == TYPE_LIST) {
        Value *value = value_create(TYPE_INT);
        value->data.intValue = arg->data.list->tail + 1;
        return value;
    }
    return error("Invalid argument type for len");
}

Value *std_range(Value *arg) {
    if (arg->type == TYPE_INT) {
        int size = arg->data.intValue;
        Value *value = value_create(TYPE_LIST);
        value->data.list = list_create(size);
        for (int i = 0; i < size; i++) {
            Value *item = value_create(TYPE_INT);
            item->data.intValue = i;
            list_add(&value->data.list, item);
        }
        return value;
    }
    return error("Invalid argument type for range");
}

StdLib parse_name(char *name) {
    if (strcmp(name, "len") == 0) { return STD_LEN; }
    if (strcmp(name, "range") == 0) { return STD_RANGE; }
    return -1;
}

int correct_arg_count(StdLib func, int arg_c) {
    switch (func) {
        case STD_LEN:   return arg_c == 1;
        case STD_RANGE: return arg_c == 1;
        default: return 1;
    }
}

Value *evaluate_std_lib_function(char *name, Value **args, int arg_c) {
    StdLib func = parse_name(name);
    if (!correct_arg_count(func, arg_c)) {
        return error("Incorrect number of arguments");
    }
    switch (func) {
        case STD_LEN: return std_len(args[0]);
        case STD_RANGE: return std_range(args[0]);
        default: return NULL; // Means function is not part of std lib
    }
}