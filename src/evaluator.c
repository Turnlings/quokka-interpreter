#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "utils/hash_table.h"
#include "utils/call_stack.h"

CallStack *callStack = NULL;

/**
 * @brief Evaluates a given AST to a return value
 * @param node The root node of the AST
 * @return The evaluated value
 */
int evaluate(ParseNode *node) {
    if (callStack == NULL) {
        callStack = malloc(sizeof(CallStack));
        stack_init(callStack);

        StackFrame main;
        main.local_variables = hash_table_create(32);
        stack_push(callStack, &main);
    }
    if (node == NULL) {
        return 0;
    }
    switch (node->type) {
        case STATEMENT_LIST:
            // Automatically make last statement the return value.
            if (node->right == NULL) {
                return evaluate(node->left);
            } else {
                evaluate(node->left);
                return evaluate(node->right);
            }
        case ASSIGNMENT:
            if (!node->left || !node->left->value.data.stringValue) {
                fprintf(stderr, "Invalid assignment target\n");
                return 0;
            }
            Value value;
            value.type = TYPE_INT; // Assume all ints for now
            value.data.intValue = evaluate(node->right);

            hash_table_set(stack_peek(callStack)->local_variables, node->left->value.data.stringValue, &value);
            break;
        case FUNCTION:
            Value func_value;
            func_value.type = TYPE_FUNCTION;
            func_value.data.node = node;

            hash_table_set(stack_peek(callStack)->local_variables, node->left->value.data.stringValue, &func_value);
            break;    
        case IDENTIFIER:
            Value id_value;
            int found = hashtable_get(stack_peek(callStack)->local_variables, node->value.data.stringValue, &id_value);
            if (found == 0) {
                fprintf(stderr, "No identifier found\nName: %s\n", node->value.data.stringValue);
            }
            switch (id_value.type) {
                case TYPE_INT:
                    return id_value.data.intValue;
                case TYPE_FUNCTION:
                    // Create new stack frame for function call
                    StackFrame frame;
                    frame.local_variables = hash_table_create(32);
                    stack_push(callStack, &frame);

                    // Get the param and arg
                    ParseNode *param = id_value.data.node->left->right;
                    ParseNode *arg = node->right;

                    // Bind parameter to argument
                    while (param && arg) {
                        Value value;
                        value.type = TYPE_INT;
                        value.data.intValue = evaluate(arg);
                        hash_table_set(stack_peek(callStack)->local_variables, 
                                    param->value.data.stringValue, 
                                    &value);

                        param = param->right;
                        arg = arg->right;
                    }

                    // Evaluate function body
                    int result = evaluate(id_value.data.node->right);
                    
                    // Clean up stack frame
                    stack_pop(callStack);
                    return result;
            }
        case LITERAL:
            return node->value.data.intValue;
        case OP_ADD:
            return evaluate(node->left) + evaluate(node->right);
        case OP_SUB:
            return evaluate(node->left) - evaluate(node->right);
        case OP_MUL:
            return evaluate(node->left) * evaluate(node->right);
        case OP_DIV:
            return evaluate(node->left) / evaluate(node->right);
        case OP_GT:
            return evaluate(node->left) > evaluate(node->right);
        case OP_GTE:
            return evaluate(node->left) >= evaluate(node->right);
        case OP_LT:
            return evaluate(node->left) < evaluate(node->right);
        case OP_LTE:
            return evaluate(node->left) <= evaluate(node->right);
        case OP_EQ:
            return evaluate(node->left) == evaluate(node->right);
        case IF:
            if ( evaluate(node->left) ) { 
                return evaluate(node->right); 
            } else {
                return 0;
            }
        default:
            fprintf(stderr, "Error evaluating Node\nType: %d\n", node->type);
            return 0;
    }
}