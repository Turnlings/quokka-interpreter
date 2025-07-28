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
    LITERAL,
    COMMENT,
    WHITESPACE,
    ASSIGNMENT,
    STATEMENT_LIST
} TokenType;

int is_operator(TokenType type);

typedef struct {
    TokenType category;
    char *text;
} Token;

typedef struct ParseNode {
    TokenType type;
    union {
        int intValue;
        char *stringValue;
    } data;
    struct ParseNode *left;
    struct ParseNode *right;
} ParseNode;

ParseNode *parse_node_create(TokenType type);

#endif