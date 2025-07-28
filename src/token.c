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

typedef struct Token {
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