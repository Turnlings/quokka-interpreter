#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "utils/hash_table.h"
#include "utils/call_stack.h"
#include "utils/file_utils.h"
#include "features/list.h"
#include "evaluator.h"
#include "lexer.h"
#include "parser.h"

#define MAX_STRING_LENGTH 128

Value *evaluate_program(ParseNode *node);
Value *evaluate_statement_list(ParseNode *node);
Value *evaluate_import(ParseNode *node);
Value *evaluate_assignment(ParseNode *node);
Value *evaluate_set(ParseNode *node);
Value *evaluate_class(ParseNode *node);
Value *evaluate_function(ParseNode *node);
Value *evaluate_list(ParseNode *node);
Value *evaluate_identifier(ParseNode *node);
Value *evaluate_while(ParseNode *node);
Value *evaluate_for(ParseNode *node);
Value *evaluate_literal(ParseNode *node);
Value *evaluate_op_add(ParseNode *node);
Value *evaluate_op_binary(ParseNode *node);
Value *evaluate_op_eq(ParseNode *node);
Value *evaluate_op_neq(ParseNode *node);
Value *evaluate_op_not(ParseNode *node);
Value *evaluate_op_index(ParseNode *node);
Value *evaluate_if(ParseNode *node);
Value *evaluate_out(ParseNode *node);
Value *evaluate_in(ParseNode *node);
Value *evaluate_return(ParseNode *node);

void runtime_error(ParseNode *node,char* string);
void cleanup();
void error_and_exit(ParseNode *node,char* string);

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

        StackFrame* main = frame_create("main");
        stack_push(callStack, main);
    }
    if (node == NULL) {
        return NULL;
    }

    switch (node->type) {
        case PROGRAM: return evaluate_program(node);
        case STATEMENT_LIST: return evaluate_statement_list(node);
        case IMPORT: return evaluate_import(node);
        case ASSIGNMENT: return evaluate_assignment(node);
        case SET: return evaluate_set(node); // Like self.identifier
        case CLASS: return evaluate_class(node);
        case FUNCTION: return evaluate_function(node);
        case LIST: return evaluate_list(node);
        case IDENTIFIER: return evaluate_identifier(node);
        case WHILE: return evaluate_while(node);
        case FOR: return evaluate_for(node);
        case LITERAL: return evaluate_literal(node);
        case OUT: return evaluate_out(node);
        case IN: return evaluate_in(node);
        case RETURN: return evaluate_return(node);
        case OP_EQ: return evaluate_op_eq(node);
        case OP_NEQ: return evaluate_op_eq(node);
        case OP_DOT: return call_object(node);
        case OP_ADD: return evaluate_op_add(node);
        case OP_INDEX: return evaluate_op_index(node);
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_MOD:
        case OP_GT:
        case OP_GTE:
        case OP_LT:
        case OP_LTE:
        case OP_AND:
        case OP_OR:
            return evaluate_op_binary(node);
        case OP_NOT: return evaluate_op_not(node);
        case TERN_IF:
        case IF:
            return evaluate_if(node);
        default:
            fprintf(stderr, "Error evaluating Node\nType: %d\nString Value: %s\n", node->type, node->value.data.stringValue);
            return NULL;
    }
}

Value *evaluate_program(ParseNode *node) {
    Value *program_return = malloc(sizeof(Value));
    program_return = value_copy(evaluate(node->right));
    cleanup();
    return program_return;
}

Value *evaluate_statement_list(ParseNode *node) {
    // Automatically make last statement the return value.
    if (node->right == NULL) {
        return evaluate(node->left);
    } else {
        if (node->left != NULL) {
            Value *value = evaluate(node->left);

            if (stack_peek(callStack)->status == 1) {
                stack_peek(callStack)->status = 0;
                return value;
            }
        }
        return evaluate(node->right);
    }
}

Value *evaluate_import(ParseNode *node) {
    // Replace the import node with the AST of the imported file
    char *input = read_file(node->left->value.data.stringValue);
    if (!input) {
        error_and_exit(node, "Invalid import statement");
    }

    int token_count = 1024; // TODO: look at linking to given count in main file
    Token *tokens = tokenize(input, &token_count);
    if (!tokens) {
        error_and_exit(node, "Failed to tokenize imported file");
    }

    ParseNode *ast = parse(tokens, token_count);
    if (!ast) {
        error_and_exit(node, "Failed to parse imported file");
    }

    // Add to the tree for easier cleanup
    node->left = ast;

    evaluate_statement_list(ast);
}

Value *evaluate_assignment(ParseNode *node) {
    if (!node->left || !node->left->value.data.stringValue) {
        runtime_error(node, "Invalid assignment target");
        return NULL;
    }
    Value *value = evaluate(node->right);
    hashtable_set(stack_peek(callStack)->local_variables, node->left->value.data.stringValue, value);
    return value;
}

Value *evaluate_set(ParseNode *node) {
    Value *rhs = evaluate(node->right);

    // Get the object
    Value *object;
    int found_object = stack_get_value(callStack, "self", &object);
    if (found_object == 0) {
        error_and_exit(node, "Set used but no class to reference");
    }

    hashtable_set(object->data.object_fields, node->left->value.data.stringValue, rhs);
    return rhs;
}

Value *evaluate_class(ParseNode *node) {
    Value *class_value = malloc(sizeof(Value));
    class_value->type = TYPE_CLASS;
    class_value->data.node = node;

    hashtable_set(stack_peek(callStack)->local_variables, node->left->value.data.stringValue, class_value);
    return class_value;
}

Value *evaluate_function(ParseNode *node) {
    Value *func_value = malloc(sizeof(Value));
    func_value->type = TYPE_FUNCTION;
    func_value->data.node = node;
    hashtable_set(stack_peek(callStack)->local_variables,
                node->left->value.data.stringValue,
                func_value);

    // Use the value in the hashtable instead
    free(func_value);
    hashtable_get(stack_peek(callStack)->local_variables,
                node->left->value.data.stringValue,
                &func_value);

    return func_value;
}

Value *evaluate_list(ParseNode *node) {
    List *list = list_create(1);
    node = node->right;
    while(node!=NULL) {
        list_add(&list, evaluate(node));
        node = node->right;
    }

    Value *list_value = malloc(sizeof(Value));
    list_value->type = TYPE_LIST;
    list_value->data.list = list;

    return list_value;
}

Value *evaluate_op_index(ParseNode *node) {
    Value *container = evaluate(node->left);
    Value *index = evaluate(node->right);

    if (container->type != TYPE_LIST) {
        runtime_error(node, "Indexing non-list");
        return NULL;
    }
    if (index->type != TYPE_INT) {
        runtime_error(node, "List index must be int");
        return NULL;
    }

    return list_access(container->data.list, index->data.intValue);
}

Value *evaluate_identifier(ParseNode *node) {
    Value *id_value;
    int found = stack_get_value(callStack, node->value.data.stringValue, &id_value);
    if (found == 0) {
        error_and_exit(node, "Identifier not yet declared");
    }
    switch (id_value->type) {
        case TYPE_FUNCTION:
            return execute_function(node, id_value);
        case TYPE_CLASS:
            return build_object(node, id_value);
        default:
            return id_value;
    }
}

Value *evaluate_while(ParseNode *node) {
    Value *value = NULL;
    while (evaluate(node->left)->data.intValue) {
        value = evaluate(node->right);
    }
    return value;
}

Value *evaluate_for(ParseNode *node) {
    Value *return_value = NULL;

    // Initialise
    evaluate(node->left->left);

    while(evaluate(node->left->right->left)->data.intValue) {
        return_value = evaluate(node->right);
        evaluate(node->left->right->right); // The change like i++;
    }
    
    return return_value;
}

Value *evaluate_literal(ParseNode *node) {
    return &node->value;
}

Value *evaluate_op_add(ParseNode *node) {
    Value *left = evaluate(node->left);
    Value *right = evaluate(node->right);
    if (left->type == TYPE_INT || left->type == TYPE_FLOAT) {
        return evaluate_op_binary(node); // TODO: look at if double evaluation left and right has side effects
    }

    Value *result = malloc(sizeof(Value));
    if (left->type == TYPE_STRING && right->type == TYPE_STRING) {
        result->type = TYPE_STRING;
        unsigned int len_left = strlen(left->data.stringValue);
        unsigned int len_right = strlen(right->data.stringValue);
        char *concat = malloc(len_left + len_right + 1);  // +1 for '\0'
        if (!concat) {
            error_and_exit(node, "Malloc Failed");
        }
        strcpy(concat, left->data.stringValue);
        strcat(concat, right->data.stringValue);
        
        result->data.stringValue = concat;
    } 
    else if (left->type == TYPE_LIST && right->type == TYPE_LIST) {
        result->type = TYPE_LIST;
        int length = left->data.list->tail + right->data.list->tail + 2;
        result->data.list = list_create(length);
        list_copy(left->data.list, result->data.list, 0);
        list_copy(right->data.list, result->data.list, left->data.list->tail + 1);
    }
    else {
        runtime_error(node, "Incompatible types for OP_ADD");
        free(result);
        return NULL;
    }
    return result;
}

Value *evaluate_op_binary_int(ParseNode *node, Value *result, int left, int right) {
    switch (node->type) {
        case OP_ADD: case OP_SUB: case OP_MUL: case OP_DIV: case OP_MOD:
            result->type = TYPE_INT;
            break;
        default:
            result->type = TYPE_BOOL;
    }
    switch (node->type) {
        case OP_ADD:
            result->data.intValue = left + right; break;
        case OP_SUB:
            result->data.intValue = left - right; break;
        case OP_MUL:
            result->data.intValue = left * right; break;
        case OP_DIV:
            result->data.intValue = left / right; break;
        case OP_MOD:
            result->data.intValue = left % right; break;
        case OP_GT:
            result->data.intValue = left > right; break;
        case OP_GTE:
            result->data.intValue = left >= right; break;
        case OP_LT:
            result->data.intValue = left < right; break;
        case OP_LTE:
            result->data.intValue = left <= right; break;
        case OP_AND:
            result->data.intValue = left && right; break;
        case OP_OR:
            result->data.intValue = left || right; break;
        default:
            runtime_error(node, "Operator not supported on integers");
    }

    return result;
}

Value *evaluate_op_binary_float(ParseNode *node, Value *result, double left, double right) {
    switch (node->type) {
        case OP_ADD: case OP_SUB: case OP_MUL: case OP_DIV:
            result->type = TYPE_FLOAT;
            break;
        default:
            result->type = TYPE_BOOL;
    }
    switch (node->type) {
        case OP_ADD:
            result->data.floatValue = left + right; break;
        case OP_SUB:
            result->data.floatValue = left - right; break;
        case OP_MUL:
            result->data.floatValue = left * right; break;
        case OP_DIV:
            result->data.floatValue = left / right; break;
        case OP_GT:
            result->data.intValue = left > right; break;
        case OP_GTE:
            result->data.intValue = left >= right; break;
        case OP_LT:
            result->data.intValue = left < right; break;
        case OP_LTE:
            result->data.intValue = left <= right; break;
        default:
            runtime_error(node, "Operator not supported on floats");
    }

    return result;
}

Value *evaluate_op_binary(ParseNode *node) {
    Value *left = evaluate(node->left);
    Value *right = evaluate(node->right);
    Value *result = malloc(sizeof(Value));

    if (left->type == TYPE_INT && right->type == TYPE_INT) {
        evaluate_op_binary_int(node, result, left->data.intValue, right->data.intValue);
        return result;
    }
    else if (left->type == TYPE_FLOAT && right->type == TYPE_FLOAT) {
        evaluate_op_binary_float(node, result, left->data.floatValue, right->data.floatValue);
        return result;
    }
    else if (left->type == TYPE_INT && right->type == TYPE_FLOAT) {
        evaluate_op_binary_int(node, result, left->data.intValue, (int)right->data.floatValue);
        return result;
    }
    else if (left->type == TYPE_FLOAT && right->type == TYPE_INT) {
        evaluate_op_binary_float(node, result, left->data.floatValue, (double)right->data.intValue);
        return result;
    }
    else if (left->type == TYPE_BOOL && right->type == TYPE_BOOL) {
        evaluate_op_binary_int(node, result, left->data.intValue, right->data.intValue);
        return result;
    }
    else {
        runtime_error(node, "Incompatible types for OPERATOR");
        return NULL;
    }
}

Value *evaluate_op_eq(ParseNode *node) { // TODO: add string support
    Value *eq = malloc(sizeof(Value));
    eq->type = TYPE_BOOL;
    eq->data.intValue = evaluate(node->left)->data.intValue == evaluate(node->right)->data.intValue;
    return eq;
}

Value *evaluate_op_neq(ParseNode *node) { // TODO: add string support
    Value *neq = malloc(sizeof(Value));
    neq->type = TYPE_BOOL;
    neq->data.intValue = evaluate(node->left)->data.intValue != evaluate(node->right)->data.intValue;
    return neq;
}

Value *evaluate_op_not(ParseNode *node) {
    Value *not = malloc(sizeof(Value));
    not->type = TYPE_BOOL;
    not->data.intValue = !evaluate(node->left)->data.intValue;
    return not;
}

Value *evaluate_if(ParseNode *node) {
    if ( evaluate(node->left)->data.intValue == 1 ) { 
        return evaluate(node->right->left); 
    } else if (node->right->right != NULL){
        return evaluate(node->right->right);
    } else {
        return NULL;
    }
}

Value *evaluate_out(ParseNode *node) {
    Value *to_out = evaluate(node->left);
    switch(to_out->type) {
        case TYPE_INT:
            printf("%d\n",to_out->data.intValue);
            break;
        case TYPE_FLOAT:
            printf(".2%f\n",to_out->data.floatValue);
            break;
        case TYPE_STRING:
            printf("%s\n",to_out->data.stringValue);
            break;
        default:
            runtime_error(node, "Invalid Output Type");
    }
    return to_out;
}

Value *evaluate_in(ParseNode *node) {
    char *line = NULL;
    size_t len = 0;
    int nread;

    printf("<< "); // Prompt              
    nread = getline(&line, &len, stdin);

    if (nread == -1) {
        runtime_error(node, "Failed to read value");
    }

    // Remove the newline character
    if (line[nread - 1] == '\n') {
        line[nread - 1] = '\0';
        nread--;
    }

    Value *in = malloc(sizeof(Value));
    in->type = TYPE_STRING;
    in->data.stringValue = line;
    return in;
}

Value *evaluate_return(ParseNode *node) {
    stack_peek(callStack)->status = 1;
    return evaluate(node->left);
}

Value *execute_function(ParseNode *node, Value *id_value) {
    // Create new stack frame for function call
    StackFrame* frame = frame_create(node->value.data.stringValue);

    // Get the param and arg
    ParseNode *param = id_value->data.node->left->right;
    ParseNode *arg = node->right;

    // Bind parameter to argument
    while (param && arg) {
        Value *value = evaluate(arg);
        hashtable_set(frame->local_variables, 
                    param->value.data.stringValue, 
                    value);

        param = param->right;
        arg = arg->right;
    }

    // Push new variables onto callstack
    stack_push(callStack, frame);

    // Evaluate function body
    Value *result = evaluate(id_value->data.node->right);
    
    // Clean up stack frame
    frame = stack_pop(callStack);
    frame_destroy(frame, 1);

    return result;
}

Value *build_object(ParseNode *node, Value *class) {
    
    HashTable *local_variables = hashtable_create(128); // TODO: make bucket size not literal

    // Get the param and arg
    ParseNode *param = class->data.node->left->right;
    ParseNode *arg = node->right;

    // Bind parameter to argument
    while (param && arg) {
        Value *value = evaluate(arg);
        hashtable_set(local_variables, 
                    param->value.data.stringValue, 
                    value);

        param = param->right;
        arg = arg->right;
    }

    // Create a frame that will be auto filled with the object fields
    StackFrame *frame = frame_create_with_variables(node->value.data.stringValue, local_variables);

    stack_push(callStack, frame);
    Value *body = evaluate(class->data.node->right);
    StackFrame *fields_stack = stack_pop(callStack);

    Value *obj = malloc(sizeof(Value));
    obj->type = TYPE_OBJECT;
    obj->data.object_fields = fields_stack->local_variables;
    frame_destroy(fields_stack, 0);

    hashtable_set(local_variables, "self", obj);

    return obj;
}

Value *call_object(ParseNode *node) {

    Value *obj = evaluate(node->left);
    if (!obj || obj->type != TYPE_OBJECT) {
        runtime_error(node, "Dot operator on non-object");
        return NULL;
    }

    // Look up field/method in object's hash table
    Value *member;
    int found = hashtable_get(obj->data.object_fields, node->right->value.data.stringValue, &member);
    if (!found) {
        runtime_error(node, "Invalid member for object");
    }

    switch (member->type) {
        case TYPE_FUNCTION:
            StackFrame* frame = frame_create_with_variables(node->value.data.stringValue, obj->data.object_fields);
            stack_push(callStack, frame);

            Value *result = execute_function(node->right, member);

            frame = stack_pop(callStack);
            frame_destroy(frame, 0);

            return result;
        default:
            return member;
    }
}

void runtime_error(ParseNode *node, char* string) {
    printf("\nRuntime Error: %s on line %d.\nCallstack:\n", string, node->line);
    stack_print(callStack);

}

void cleanup() {
    stack_destroy(callStack);
}

void error_and_exit(ParseNode *node, char* string) {
    runtime_error(node, string);
    cleanup();
    exit(1);
}