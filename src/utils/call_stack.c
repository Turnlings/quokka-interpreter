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
    StackFrame **frames;
    int top;
} CallStack;

void frame_init(StackFrame *frame) {
    frame->return_address = NULL;
    frame->local_variables = hashtable_create(32);
} 

void stack_init(CallStack *stack) {
    stack->frames = malloc(sizeof(StackFrame*) * MAX_FRAMES);
    stack->top = -1;
}

void stack_push(CallStack *stack, StackFrame *frame) {
    // TODO: free any stackframe that may be overwitten
    printf("STACK TOP E: %d\n", stack->top);
    if (stack->top + 1 < MAX_FRAMES) {
        stack->frames[++stack->top] = frame;
    } else {
        fprintf(stderr, "Stack Overflow!!!");
        exit(1);
    }
    printf("STACK TOP F: %d\n", stack->top);
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
    return stack->frames[stack->top];
}

// Descend stack to get all
int stack_get_value(CallStack *stack, const char *key, Value *out_value) {
    for (int i = stack->top; i >= 0; i--) {
        hashtable_get(stack->frames[i]->local_variables, key, out_value);
        if (out_value != NULL) {
            return 1;
        }
    }
    return 0;
}