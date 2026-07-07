#include <string.h>
#include "token.h"
#include "features/list.h"

typedef enum {
    STD_LEN,
    STD_RANGE,
    STD_SUM,
    STD_ERROR,
    STD_HANDLE_ERROR,
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

Value *std_sum(Value *arg) {
    if (arg->type == TYPE_LIST) {
        if (arg->data.list->tail >= 0) {
            ValueType type = list_access(arg->data.list, 0)->type;
            int sum_i = 0;
            float sum_f = 0.0f;
            
            for (int i = 0; i <= arg->data.list->tail; i++) {
                Value *item = list_access(arg->data.list, i);
                if (item->type != type) {
                    return error("Inconsistent types in list for sum");
                }
                if (item->type == TYPE_INT) {
                    sum_i += item->data.intValue;
                }
                if (item->type == TYPE_FLOAT) {
                    sum_f += item->data.floatValue;
                }
            }

            if (type == TYPE_INT) {
                Value *value = value_create(TYPE_INT);
                value->data.intValue = sum_i;
                return value;
            }
            if (type == TYPE_FLOAT) {
                Value *value = value_create(TYPE_FLOAT);
                value->data.floatValue = sum_f;
                return value;
            }
        }
    }
    return error("Invalid argument type for sum");
}

Value *std_error(Value *arg) {
    if (arg->type != TYPE_STRING) {
        return error("Invalid argument type for error");
    }
    return error(arg->data.stringValue);
}

Value *std_handle_error(Value *arg) {
    Value *value = value_create(TYPE_BOOL);
    int is_error = arg->type == TYPE_ERROR;
    value->data.intValue = (is_error);

    if (is_error) {
        arg->type = TYPE_ERROR_HANDLED;
    }

    return value;
}

StdLib parse_name(char *name) {
    if (strcmp(name, "len") == 0) { return STD_LEN; }
    if (strcmp(name, "range") == 0) { return STD_RANGE; }
    if (strcmp(name, "sum") == 0) { return STD_SUM; }
    if (strcmp(name, "handle") == 0) { return STD_HANDLE_ERROR; }
    if (strcmp(name, "error") == 0) { return STD_ERROR; }
    return -1;
}

int correct_arg_count(StdLib func, int arg_c) {
    switch (func) {
        case STD_LEN:      return arg_c == 1;
        case STD_RANGE:    return arg_c == 1;
        case STD_SUM:      return arg_c == 1;
        case STD_HANDLE_ERROR: return arg_c == 1;
        case STD_ERROR:    return arg_c == 1;
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
        case STD_SUM: return std_sum(args[0]);
        case STD_HANDLE_ERROR: return std_handle_error(args[0]);
        case STD_ERROR: return std_error(args[0]);
        default: return NULL; // Means function is not part of std lib
    }
}