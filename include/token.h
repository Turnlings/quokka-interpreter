#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    IDENTIFIER,
    KEYWORD,
    SEPERATOR,
    OPERATOR,
    LITERAL,
    COMMENT,
    WHITESPACE,
    ASSIGNMENT,
    STATEMENT_LIST
} TokenType;

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