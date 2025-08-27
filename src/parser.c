#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "token.h"

Token* input_tokens;
int position;
Token current_t;
int count;

void syntax_error(char* string) {
    printf("%s\nAt token: %d\n", string, position);
}

static void advance() {
    current_t = input_tokens[++position];
}

static Token peek() {
    return input_tokens[position + 1];
}

static void expect(TokenType expected) {
    if (current_t.category == expected) {
        advance();
    } else {
        syntax_error("Unexpected token type");
    }
}

static bool match(TokenType match) {
    return current_t.category == match;
}

ParseNode *parse_identifier() {
    if (match(IDENTIFIER)) {
        ParseNode *node = parse_node_create(IDENTIFIER);
        node->value.data.stringValue = current_t.text;
        advance();
        return node;
    } else {
        syntax_error("Expected Literal");
        return NULL;
    }
}

ParseNode *parse_literal() {
    if (match(LITERAL)) {
        ParseNode *node = parse_node_create(LITERAL);
        node->value.data.intValue = atoi(current_t.text);
        advance();
        return node;
    } else {
        syntax_error("Expected Literal");
        return NULL;
    }
}

ParseNode *parse_term() {
    if (match(IDENTIFIER)) {
        return parse_identifier();
    }
    return parse_literal();
}

ParseNode *parse_assignment() {
    if (match(ASSIGNMENT)) {
        ParseNode *node = parse_node_create(ASSIGNMENT);
        node->value.data.stringValue = strdup(current_t.text);
        advance();
        return node;
    }
}

ParseNode *parse_operator() {
    if (is_operator(current_t.category)) {
        ParseNode *node = parse_node_create(current_t.category);
        node->value.data.stringValue = current_t.text;
        advance();
        return node;
    } else {
        syntax_error("Expected Operator");
        return NULL;
    }
}

ParseNode *parse_expression() {
    if (peek().category == ASSIGNMENT) {
        ParseNode* left = parse_term();
        ParseNode* assignment = parse_assignment();
        ParseNode* right = parse_expression();

        assignment->left = left;
        assignment->right = right;

        return assignment;
    } else if (is_operator(peek().category)) { // Operator
        ParseNode* left = parse_term();
        ParseNode* operator = parse_operator();
        ParseNode* right = parse_term();

        operator->left = left;
        operator->right = right;

        return operator;
    } else {
        return parse_term();
    }
}

ParseNode *parse(Token *input, int size) {
    input_tokens = input;
    count = size;
    position = 0;
    current_t = input_tokens[position];

    return parse_expression();
}