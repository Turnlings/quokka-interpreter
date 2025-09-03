#ifndef CALL_STACK_H
#define CALL_STAKC_H

#include "utils/hash_table.h"
#include "token.h"

typedef struct StackFrame {
    ParseNode *return_address;
    HashTable *local_variables;
    int status;
} StackFrame;

typedef struct CallStack {
    StackFrame **frames;
    int top;
} CallStack;

StackFrame* frame_create();
void frame_destroy(StackFrame *frame, int destroy_hashtable);

void stack_init(CallStack *stack);
void stack_push(CallStack *stack, StackFrame *frame);
StackFrame *stack_pop(CallStack *stack);
StackFrame *stack_peek(CallStack *stack);
int stack_get_value(CallStack *stack, const char *key, Value *out_value);

#endif