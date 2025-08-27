#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "token.h"

ParseNode *parse_expression();
ParseNode *add_child(ParseNode *parent, ParseNode *child);

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

ParseNode *parse_args() {
    ParseNode* root = NULL;
    expect(PAREN_L);
    while (!match(PAREN_R)) {
        ParseNode* node = parse_expression();
        root = add_child(root, node);
        if (match(COMMA)) {
            advance();
        }
    }
    advance();
    return root;
}

ParseNode *parse_identifier() {
    if (match(IDENTIFIER)) {
        ParseNode *node = parse_node_create(IDENTIFIER);
        node->value.data.stringValue = current_t.text;
        advance();

        if (match(PAREN_L)) {
            ParseNode *args = parse_args();
            node->right = args;
        }
        return node;
    } else {
        syntax_error("Expected Idnetifier");
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

ParseNode *parse_while() {
    expect(WHILE);
    ParseNode *condition = parse_expression();
    expect(DO);
    ParseNode *body = parse_expression();
    ParseNode *node = parse_node_create(WHILE);
    node->left = condition;
    node->right = body;
    return node;
}

ParseNode *parse_function_defintion() {
    expect(DEF);
    ParseNode *identifier = parse_expression();
    expect(FUNCTION);
    ParseNode *body = parse_expression();

    ParseNode *node = parse_node_create(FUNCTION);
    node->left = identifier;
    node->right = body;
    return node;
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
    if (match(DEF)) {
        return parse_function_defintion();
    } else if (match(WHILE)) {
        return parse_while();
    } else if (peek().category == ASSIGNMENT) {
        ParseNode* left = parse_term();
        ParseNode* assignment = parse_assignment();
        ParseNode* right = parse_expression();

        assignment->left = left;
        assignment->right = right;

        return assignment;
    } else if (is_operator(peek().category)) { // Operator
        ParseNode* left = parse_term();
        ParseNode* operator = parse_operator();
        ParseNode* right = parse_expression();

        operator->left = left;
        operator->right = right;

        return operator;
    } else {
        return parse_term();
    }
}

ParseNode *add_child(ParseNode *parent, ParseNode *child) {
    if (parent == NULL) return child;  // return new root

    ParseNode *cur = parent;
    while (cur->right != NULL) {
        cur = cur->right;
    }
    cur->right = child;
    return parent;
}

ParseNode *parse_program() {
    ParseNode *root = parse_node_create(STATEMENT_LIST);

    while (position < count) {
        ParseNode *node = parse_node_create(STATEMENT_LIST);
        ParseNode *expr = parse_expression();
        node->left = expr;
        root = add_child(root, node);

        if (current_t.category == SEPERATOR) {
            advance();
        } else {
            syntax_error("Missing semi-colon");
        }
    }

    return root;
}

ParseNode *parse(Token *input, int size) {
    input_tokens = input;
    count = size;
    position = 0;
    current_t = input_tokens[position];

    return parse_program();
}