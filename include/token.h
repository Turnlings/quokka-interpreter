#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    IDENTIFIER,
    KEYWORD,
    SEPERATOR,
    OPERATOR,
    LITERAL,
    COMMENT,
    WHITESPACE
} TokenType;

typedef struct {
    TokenType category;
    char *text;
} Token;

typedef struct {
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