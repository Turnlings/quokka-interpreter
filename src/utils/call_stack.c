#include <stdlib.h>
#include <stdio.h>
#include "utils/hash_table.h"
#include "token.h"

#define MAX_FRAMES 32

typedef struct StackFrame {
    ParseNode *return_address;
    HashTable *local_variables;
} StackFrame;

typedef struct CallStack {
    StackFrame frames[MAX_FRAMES];
    int top;
} CallStack;

void stack_init(CallStack *stack) {
    stack->top = -1;
}

void stack_push(CallStack *stack, StackFrame *frame) {
    // TODO: free any stackframe that may be overwitten
    if (stack->top + 1 < MAX_FRAMES) {
        stack->frames[++stack->top] = *frame;
    } else {
        fprintf(stderr, "Stack Overflow!!!");
        exit(1);
    }
}

StackFrame *stack_pop(CallStack *stack) {
    if (stack->top >= 0) {
        return &stack->frames[stack->top--];
    } else {
        fprintf(stderr, "Stack Underflow");
        exit(1);
    }
}

StackFrame *stack_peek(CallStack *stack) {
    return &stack->frames[stack->top];
}