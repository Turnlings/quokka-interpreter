#include <stdio.h>
#include "token.h"

/**
 * @brief Evaluates a given AST to a return value
 * @param node The root node of the AST
 * @return The evaluated value
 */
int evaluate(ParseNode *node) {
    switch (node->type) {
        case STATEMENT_LIST:
            return evaluate(node->left); // TODO: temp for single statement
        case LITERAL:
            return node->data.intValue;
        case OPERATOR:
            return evaluate(node->left) + evaluate(node->right);
        default:
            fprintf(stderr, "Error evaluating Node\n");
            return NULL;
    }
}