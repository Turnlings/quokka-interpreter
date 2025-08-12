#ifndef TOKEN_H
#define TOKEN_H

typedef enum TokenType {
    IDENTIFIER,
    KEYWORD,
    SEPERATOR,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    RETURN,
    LITERAL,
    COMMENT,
    WHITESPACE,
    ASSIGNMENT,
    STATEMENT_LIST,

    // For if statments
    IF,
    THEN,
    END
} TokenType;

int is_operator(TokenType type);

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

ParseNode *parse_node_create(TokenType type);

#endif