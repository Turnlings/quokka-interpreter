#include <stdlib.h>
#include <string.h>
#include "token.h"

/**
 * parse_expression - converts array of tokens into a parse tree 
 * 
 * @param tokens The array of tokens that has already passed through the lexer
 * @param count  The length of the array
 * 
 * @return The root of the parse tree
 */
ParseNode *parse_expression(Token *tokens, int count) {
    // Base cases
    if (count == 1) {
        if (tokens[0].category == LITERAL) {
            ParseNode *node = parse_node_create(LITERAL);
            node->data.intValue = atoi(tokens[0].text);
            return node;
        }
        if (tokens[0].category == IDENTIFIER) {
            ParseNode *node = parse_node_create(IDENTIFIER);
            node->data.stringValue = tokens[0].text;
            return node;
        }

        // TODO: if reaches here something has gone wrong so how do I want to 
        // handle that?
    }

    // Recursive cases
    for (int i = 0; i < count; i++) {
        if (tokens[i].category == OPERATOR ) { //&& strcmp(tokens[i].text, "+") == 0) {
            ParseNode *node = parse_node_create(OPERATOR);
            node->data.stringValue = strdup(tokens[i].text);

            // Recursively parse left and right expressions
            node->left = parse_expression(tokens, i);
            node->right = parse_expression(tokens + i + 1, count - i - 1);

            return node;
        }
    }

    // If nothing found then invalid expression, end this branch
    return NULL;
}

int get_precedence(const char *op) {
    if (strcmp(op, "<-") == 0) return 4;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0) return 2;
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    return 0;
}