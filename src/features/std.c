#include <string.h>
#include "token.h"

typedef enum {
    STD_LEN
} StdLib;

Value *std_len(Value *arg) {
    if (arg->type == TYPE_STRING) {
        Value *value = value_create(TYPE_INT);
        value->data.intValue = strlen(arg->data.stringValue);
        return value;
    }
    return NULL;
}

StdLib parse_name(char *name) {
    printf("Parsing function: %s\n", name);
    if (strcmp(name, "len") == 0) { return STD_LEN; }
    return -1;
}

Value *evaluate_std_lib_function(char *name, Value **args) {
    StdLib func = parse_name(name);
    switch (func) {
        case STD_LEN: return std_len(args[0]);
        default: return NULL;
    }
}