#ifndef CALL_STACK_H
#define CALL_STAKC_H

#include "utils/hash_table.h"
#include "token.h"

typedef struct StackFrame {
    ParseNode *return_address;
    HashTable local_variables;
} StackFrame;

typedef struct CallStack {
    StackFrame* frames;
    int top;
} CallStack;

void stack_init(CallStack *stack);
void stack_push(CallStack *stack, StackFrame frame);
StackFrame stack_pop(CallStack *stack);

#endif