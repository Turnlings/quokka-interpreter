#include "token.h"

int is_operator(TokenType type) {
    return type == OP_ADD || type == OP_SUB || type == OP_MUL || type == OP_DIV ||
           type == OP_GT || type == OP_GTE || type == OP_LT || type == OP_LTE ||
           type == OP_EQ || type == OP_DOT;
}

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

void free_ast(ParseNode *node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    free(node);
}