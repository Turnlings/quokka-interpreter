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
    OP_ADD, OP_ADD_EQUALS, OP_ADD_ADD,
    OP_SUB, OP_SUB_EQUALS, OP_SUB_SUB,
    OP_MUL, OP_MUL_EQUALS,
    OP_DIV, OP_DIV_EQUALS,
    OP_MOD, OP_MOD_EQUALS,

    // Comparison operators
    OP_GT, OP_GTE,
    OP_LT, OP_LTE,
    OP_EQ, OP_NEQ,

    // Other operators
    OP_DOT,
    ASSIGNMENT,

    // Logical states and operators
    TRUE, FALSE,
    OP_AND, OP_OR, OP_NOT,

    // Literals
    RETURN,
    LITERAL,
    FLOAT,
    STRING,
    LIST,
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
    SQUARE_L, SQUARE_R,
    COMMA,

    // I/O
    IN,
    OUT,

    // Misc
    CONTROL
} TokenType;

int is_operator(TokenType type);
int is_compound_assignment_operator(TokenType type);

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_LIST,
    TYPE_FUNCTION,
    TYPE_CLASS,
    TYPE_OBJECT
} ValueType;

typedef struct ParseNode ParseNode;
typedef struct List List;

typedef struct Value {
    ValueType type;
    union {
        int intValue;
        double floatValue;
        char *stringValue;
        ParseNode *node;
        HashTable *object_fields;
        List *list;
    } data;
} Value;

typedef struct Token {
    TokenType category;
    char *text;
    int line;
} Token;

struct ParseNode {
    TokenType type;
    Value value;
    struct ParseNode *left;
    struct ParseNode *right;
    int line;
};

ParseNode *parse_node_create(TokenType type);
void print_ast(ParseNode *node);
void free_ast(ParseNode *node);

#endif