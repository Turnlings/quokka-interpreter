#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "parser.h"

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

    // Assignment: right-associative, so parse from right to left
    for (int i = count - 1; i >= 0; i--) {
        if (tokens[i].category == ASSIGNMENT) {
            ParseNode *node = parse_node_create(ASSIGNMENT);
            node->data.stringValue = strdup(tokens[i].text); // should be "<-"
            node->left = parse_expression(tokens, i);
            node->right = parse_expression(tokens + i + 1, count - i - 1);
            return node;
        }
    }

    // TODO: look at precedence climbing as currently does not handle BIDMAS

    // Operators: left-associative, so parse from left to right
    for (int i = 0; i < count; i++) {
        TokenType type = tokens[i].category;

        if (is_operator(type)) {
            ParseNode *node = NULL;

            switch (type) {
                case OP_ADD:
                    node = parse_node_create(OP_ADD);
                    break;
                case OP_SUB:
                    node = parse_node_create(OP_SUB);
                    break;
                case OP_MUL:
                    node = parse_node_create(OP_MUL);
                    break;
                case OP_DIV:
                    node = parse_node_create(OP_DIV);
                    break;
            }
            node->data.stringValue = tokens[i].text;
            node->left = parse_expression(tokens, i);
            node->right = parse_expression(tokens + i + 1, count - i - 1);
            return node;
        }
    }

    // If nothing found then expression is invalid
    return NULL;
}

int get_precedence(const char *op) {
    if (strcmp(op, "<-") == 0) return 4;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0) return 2;
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    return 0;
}