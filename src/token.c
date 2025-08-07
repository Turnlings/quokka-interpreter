#include <stdio.h>
#include <stdlib.h>

/*
* The tokens passed from the lexer to the syntax tree
*/

typedef enum {
    IDENTIFIER,
    KEYWORD,
    SEPERATOR,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    LITERAL,
    COMMENT,
    WHITESPACE,
    ASSIGNMENT,
    STATEMENT_LIST
} TokenType;

int is_operator(TokenType type) {
    return type == OP_ADD || type == OP_SUB || type == OP_MUL || type == OP_DIV;
}

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
} ValueType;

typedef struct Value {
    ValueType type;
    union {
        int intValue;
        float floatValue;
        char *stringValue;
    } data;
} Value;

typedef struct Token {
    TokenType category;
    char *text;
} Token;

typedef struct ParseNode {
    TokenType type;
    Value value;
    struct ParseNode *left;
    struct ParseNode *right;
} ParseNode;

ParseNode *parse_node_create(TokenType type){
    ParseNode *node = malloc(sizeof(ParseNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    node->type = type;

    node->left = NULL;
    node->right = NULL;
    return node;
}