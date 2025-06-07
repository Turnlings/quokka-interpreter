/*
* The tokens passed from the lexer to the syntax tree
*/

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
    char category;
    char *text;
} Token;