#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "utils/hash_table.h"
#include "utils/call_stack.h"
#include "evaluator.h"

#define MAX_STRING_LENGTH 128

void runtime_error(char* string) {
    printf("Runtime Error: %s\n", string);
}

CallStack *callStack = NULL;

/**
 * @brief Evaluates a given AST to a return value
 * @param node The root node of the AST
 * @return The evaluated value
 */
Value *evaluate(ParseNode *node) {
    if (callStack == NULL) {
        callStack = malloc(sizeof(CallStack));
        stack_init(callStack);

        StackFrame* main = malloc(sizeof(StackFrame));
        frame_init(main);
        stack_push(callStack, main);
    }
    if (node == NULL) {
        return NULL;
    }

    switch (node->type) {
        case STATEMENT_LIST:
            // Automatically make last statement the return value.
            if (node->right == NULL) {
                return evaluate(node->left);
            } else {
                if (node->left != NULL) {
                    evaluate(node->left);
                }
                return evaluate(node->right);
            }
        case ASSIGNMENT:
            if (!node->left || !node->left->value.data.stringValue) {
                fprintf(stderr, "Invalid assignment target\n");
                return NULL;
            }
            Value *value = evaluate(node->right);
            hashtable_set(stack_peek(callStack)->local_variables, node->left->value.data.stringValue, value);
            break;
        case FUNCTION:
            Value func_value;
            func_value.type = TYPE_FUNCTION;
            func_value.data.node = node;

            hashtable_set(stack_peek(callStack)->local_variables, node->left->value.data.stringValue, &func_value);
            break;    
        case IDENTIFIER:
            Value *id_value = malloc(sizeof(Value));
            int found = stack_get_value(callStack, node->value.data.stringValue, id_value);
            if (found == 0) {
                fprintf(stderr, "Error: %s not yet declared.\n", node->value.data.stringValue);
                exit(1);
            }
            switch (id_value->type) {
                case TYPE_FUNCTION:
                    return execute_function(node, id_value);
                default:
                    return id_value;
            }
        case WHILE:
            while (evaluate(node->left)) {
                evaluate(node->right);
            }
        case LITERAL:
            return &node->value;
        case OP_ADD:
            Value *left_a = evaluate(node->left);
            Value *right_a = evaluate(node->right);
            Value *result_a = malloc(sizeof(Value));
            if (left_a->type == TYPE_INT && right_a->type == TYPE_INT) {
                result_a->type = TYPE_INT;
                result_a->data.intValue = left_a->data.intValue + right_a->data.intValue;
            }
            else if (left_a->type == TYPE_STRING && right_a->type == TYPE_STRING) {
                result_a->type = TYPE_STRING;
                unsigned int len_left = strlen(left_a->data.stringValue);
                unsigned int len_right = strlen(right_a->data.stringValue);
                char *concat = malloc(len_left + len_left + 1);  // +1 for '\0'
                if (!concat) {
                    runtime_error("Malloc Failed");
                    exit(1);
                }
                strcpy(concat, left_a->data.stringValue);
                strcat(concat, right_a->data.stringValue);
                
                result_a->data.stringValue = concat;
            } 
            else {
                runtime_error("Incompatible types for OP_ADD");
                return NULL;
            }
            return result_a;
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_GT:
        case OP_GTE:
        case OP_LT:
        case OP_LTE:
            Value *left = evaluate(node->left);
            Value *right = evaluate(node->right);
            Value *result;

            if (left->type == TYPE_INT && right->type == TYPE_INT) {
                result->type = TYPE_INT;

                switch (node->type) {
                    case OP_SUB:
                        result->data.intValue = left->data.intValue - right->data.intValue;
                    case OP_MUL:
                        result->data.intValue = left->data.intValue * right->data.intValue;
                    case OP_DIV:
                        result->data.intValue = left->data.intValue / right->data.intValue;
                    case OP_GT:
                        result->data.intValue = left->data.intValue > right->data.intValue;
                    case OP_GTE:
                        result->data.intValue = left->data.intValue >= right->data.intValue;
                    case OP_LT:
                        result->data.intValue = left->data.intValue < right->data.intValue;
                    case OP_LTE:
                        result->data.intValue = left->data.intValue <= right->data.intValue;
                }

                return result;
            }
            else {
                runtime_error("Incompatible types for OPERATOR");
                return NULL;
            }
        case OP_EQ:
            return evaluate(node->left) == evaluate(node->right);
        case TERN_IF:
        case IF:
            if ( evaluate(node->left) ) { 
                return evaluate(node->right->left); 
            } else if (node->right->right != NULL){
                return evaluate(node->right->right);
            } else {
                return NULL;
            }
        default:
            fprintf(stderr, "Error evaluating Node\nType: %d\n", node->type);
            return 0;
    }
}

Value *execute_function(ParseNode *node, Value *id_value) {
    // Create new stack frame for function call
    StackFrame* frame = malloc(sizeof(StackFrame));
    frame->local_variables = hashtable_create(32);

    // Get the param and arg
    ParseNode *param = id_value->data.node->left->right;
    ParseNode *arg = node->right;

    // Bind parameter to argument
    while (param && arg) {
        Value *value = evaluate(arg);
        hashtable_set(frame->local_variables, 
                    param->value.data.stringValue, 
                    value);

        printf("Param: %s\nArg: %d\n", param->value.data.stringValue, value->data.intValue);

        param = param->right;
        arg = arg->right;
    }

    // Push new variables onto callstack
    stack_push(callStack, frame);

    // Evaluate function body
    Value *result = evaluate(id_value->data.node->right);
    
    // Clean up stack frame
    stack_pop(callStack);

    return result;
}