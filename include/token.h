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
    struct ParseNode **children;
    int child_count;
} ParseNode;

#endif