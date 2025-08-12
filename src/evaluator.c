#include <stdio.h>
#include "token.h"
#include "utils/hash_table.h"

/**
 * @brief Evaluates a given AST to a return value
 * @param node The root node of the AST
 * @return The evaluated value
 */
int evaluate(ParseNode *node, HashTable *symbolTable) {
    if (node == NULL) {
        return 0;
    }
    switch (node->type) {
        case STATEMENT_LIST:
            // Automatically make last statement the return value.
            if (node->right == NULL) {
                return evaluate(node->left, symbolTable);
            } else {
                evaluate(node->left, symbolTable);
                return evaluate(node->right, symbolTable);
            }
        case ASSIGNMENT:
            if (!node->left || !node->left->value.data.stringValue) {
                fprintf(stderr, "Invalid assignment target\n");
                return 0;
            }
            Value value;
            value.type = TYPE_INT; // Assume all ints for now
            int intValue = evaluate(node->right, symbolTable);
            value.data.intValue = intValue;

            hash_table_set(symbolTable, node->left->value.data.stringValue, &value);
            break;
        case IDENTIFIER:
            Value id_value;
            int found = hashtable_get(symbolTable, node->value.data.stringValue, &id_value);
            if (found == 0) {
                fprintf(stderr, "No identifier found\nName: %s\n", node->value.data.stringValue);
            }
            return id_value.data.intValue; // Assume all ints for now
        case LITERAL:
            return node->value.data.intValue;
        case OP_ADD:
            return evaluate(node->left, symbolTable) + evaluate(node->right, symbolTable);
        case OP_SUB:
            return evaluate(node->left, symbolTable) - evaluate(node->right, symbolTable);
        case OP_MUL:
            return evaluate(node->left, symbolTable) * evaluate(node->right, symbolTable);
        case OP_DIV:
            return evaluate(node->left, symbolTable) / evaluate(node->right, symbolTable);
        case IF:
            if ( evaluate(node->left, symbolTable) ) { 
                return evaluate(node->right, symbolTable); 
            } else {
                return 0;
            }
        default:
            fprintf(stderr, "Error evaluating Node\nType: %d\n", node->type);
            return NULL;
    }
}