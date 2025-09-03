#include <stdio.h>
#include <stdlib.h>

typedef struct HashTable HashTable;

/*
* The tokens passed from the lexer to the syntax tree
*/

typedef enum {
    PROGRAM,
    IDENTIFIER,
    KEYWORD,
    SEPERATOR,

    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,

    OP_GT, OP_GTE,
    OP_LT, OP_LTE,
    OP_EQ,

    OP_DOT,

    RETURN,
    LITERAL,
    STRING,
    COMMENT,
    WHITESPACE,
    ASSIGNMENT,
    STATEMENT_LIST,

    // For if statments
    TERN_IF,
    TERN_COLON,

    PAREN_L,
    PAREN_R,

    BRACES_L,
    BRACES_R,
    BLOCK,

    DEF,
    FUNCTION,
    COMMA,

    IF,
    ELSE,
    WHILE,
    DO,

    IN,
    OUT,

    CLASS,
    SET
} TokenType;

int is_operator(TokenType type) {
    return type == OP_ADD || type == OP_SUB || type == OP_MUL || type == OP_DIV ||
           type == OP_GT || type == OP_GTE || type == OP_LT || type == OP_LTE ||
           type == OP_EQ || type == OP_DOT;
}

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_FUNCTION,
    TYPE_CLASS,
    TYPE_OBJECT
} ValueType;

typedef struct ParseNode ParseNode;

typedef struct Value {
    ValueType type;
    union {
        int intValue;
        float floatValue;
        char *stringValue;
        ParseNode *node;
        HashTable *object_fields;
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