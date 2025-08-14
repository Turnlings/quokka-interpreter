#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "parser.h"

ParseNode *base_cases(Token *tokens);

ParseNode* parse_statements(Token* tokens, int count) {
    if (count <= 0) return NULL;
    int start = 0;
    ParseNode* list = NULL;
    ParseNode* last = NULL;
    for (int i = 0; i < count; i++) {
        if (tokens[i].category == SEPERATOR) {
            int stmt_len = i - start;
            if (stmt_len > 0) {
                ParseNode* stmt = parse_expression(tokens + start, stmt_len);
                ParseNode* node = parse_node_create(STATEMENT_LIST);
                node->left = stmt;
                node->right = NULL;
                if (!list) {
                    list = node;
                } else {
                    last->right = node;
                }
                last = node;
            }
            start = i + 1;
        }
    }

    return list;
}

/**
 * parse_expression - converts array of tokens into a parse tree 
 * 
 * @param tokens The array of tokens that has already passed through the lexer
 * @param count  The length of the array
 * 
 * @return The root of the parse tree
 */
ParseNode *parse_expression(Token *tokens, int count) {
    if (count == 0) {
        fprintf(stderr, "Passed an empty token to parse_expression\n");
        return NULL;
    }
    if (count == 1) {
        return base_cases(tokens);
    }
    printf("Count: %d\n", count);

    // First check if parenthesis
    if (tokens[0].category == PAREN_L) {
        printf("HERE1\n");
        int depth = 1;
        for (int i = 1; i < count; i++) {
            printf("I: %d\n",i);
            if (tokens[i].category == PAREN_L) depth++;
            else if (tokens[i].category == PAREN_R) depth--;
            if (depth == 0) {
                printf("HERE2\n");
                // Everthing up to i goes in a statement list
                ParseNode *node = parse_expression(tokens + 1, i - 1);
            
                // If nothing to the right then can just return
                if (i == count - 1) {

                    return node;
                }

                // Otherwise, the right is the parent so return that
                ParseNode *parent = parse_expression(tokens + i + 1, count - i - 1);
                parent->left = node; // In theory the left should have failed to evaluate
                return parent;
            }
        }
    }

    // Assignment: right-associative, so parse from right to left
    for (int i = count - 1; i >= 0; i--) {
        if (tokens[i].category == ASSIGNMENT) {
            ParseNode *node = parse_node_create(ASSIGNMENT);
            node->value.data.stringValue = strdup(tokens[i].text);
            node->left = parse_expression(tokens, i);
            node->right = parse_expression(tokens + i + 1, count - i - 1);
            return node;
        }
    }

    for (int i = 0; i < count; i++) {
        TokenType type = tokens[i].category;
        switch (type) {
            ParseNode *node;
            case FUNCTION:
                node = parse_node_create(FUNCTION);
                node->value.data.stringValue = strdup(tokens[i].text);
                node->left = parse_expression(tokens, i);
                node->right = parse_expression(tokens + i + 1, count - i - 1);
                return node;
            case IF:
                node = parse_node_create(IF);
                node->value.data.stringValue = strdup(tokens[i].text);
                node->left = parse_expression(tokens, i);
                node->right = parse_expression(tokens + i + 1, count - i - 1);
                return node;
        }
    }

    // Function call: identifier followed by expressions
    // Count is > 1 otherwise would have already returned
    // TODO: there is probably a better way of handling
    if (tokens[0].category == IDENTIFIER) {
        ParseNode *node = parse_node_create(IDENTIFIER);
        node->value.data.stringValue = tokens[0].text;
        // Parse arguments
        node->right = parse_expression(tokens + 1, count - 1);
        return node;
    }

    for (int i = 0; i < count; i++) {
        TokenType type = tokens[i].category;

        if (is_operator(type)) {
            ParseNode *node = NULL;
            node = parse_node_create(type);
            node->value.data.stringValue = tokens[i].text;
            node->left = parse_expression(tokens, i);
            node->right = parse_expression(tokens + i + 1, count - i - 1);
            return node;
        }
    }

    // For function args
    if (tokens[0].category == LITERAL) {
        ParseNode *node = parse_node_create(LITERAL);
        node->value.data.intValue = atoi(tokens[0].text);
        // Chain args
        node->right = parse_expression(tokens + 1, count - 1);
        return node;
    }

    return NULL;
}

int get_precedence(TokenType category) {
    switch(category) {
        case OP_ADD:
        case OP_SUB: return 1;
        case OP_MUL:
        case OP_DIV: return 2;
        default: return 0;
    }
}

ParseNode *base_cases(Token *tokens) {
    if (tokens[0].category == LITERAL) {
        ParseNode *node = parse_node_create(LITERAL);
        node->value.data.intValue = atoi(tokens[0].text);
        return node;
    }
    if (tokens[0].category == IDENTIFIER) {
        ParseNode *node = parse_node_create(IDENTIFIER);
        node->value.data.stringValue = tokens[0].text;
        return node;
    }

    return NULL; // Invalid!!!
}