#include <stdlib.h>
#include "token.h"

/**
 * parse_tokens - converts array of tokens into a parse tree 
 * 
 * @param tokens The array of tokens that has already passed through the lexer
 * @param count  The length of the array
 * 
 * @return The root of the parse tree
 */
ParseNode *parse_tokens(Token* tokens, int count) {
    for (int i=0; i < count; i++){
        if (tokens[i].category == OPERATOR && strcmp(tokens[i].text, "+") == 0) {
            // Create operator node
            ParseNode *node = parse_node_create(OPERATOR);
            strcpy(node->data.stringValue, tokens[i].text); // TODO: make sure copy is done properly

            node->left = parse_expression(tokens, i);

            node->right = parse_expression(tokens + i + 1, count - i - 1);

            return node;
        }
    }

    // Assume a literal if nothing found
    // TODO: make safe
    ParseNode *node = parse_node_create(LITERAL);
    node->data.intValue = atoi(tokens[0].text);
    return node;
}