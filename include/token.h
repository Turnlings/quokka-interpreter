#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>

typedef struct HashTable HashTable;

typedef enum {
    // General
    PROGRAM,
    IDENTIFIER,
    KEYWORD,
    SEPERATOR,

    // Arithmetic operators
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,

    // Comparison operators
    OP_GT, OP_GTE,
    OP_LT, OP_LTE,
    OP_EQ,

    // Other operators
    OP_DOT,
    ASSIGNMENT,

    // Literals
    RETURN,
    LITERAL,
    STRING,
    COMMENT,
    WHITESPACE,

    // Control flow
    IF,
    ELSE,
    WHILE,
    DO,
    FOR,
    TERN_IF,
    TERN_COLON,

    // Structure
    STATEMENT_LIST,
    BLOCK,
    FUNCTION,
    DEF,
    CLASS,
    SET,

    // Symbols
    PAREN_L, PAREN_R,
    BRACES_L, BRACES_R,
    COMMA,

    // I/O
    IN,
    OUT,

    // Misc
    CONTROL
} TokenType;

int is_operator(TokenType type);

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

struct ParseNode {
    TokenType type;
    Value value;
    struct ParseNode *left;
    struct ParseNode *right;
};

ParseNode *parse_node_create(TokenType type);
void print_ast(ParseNode *node);
void free_ast(ParseNode *node);

#endif