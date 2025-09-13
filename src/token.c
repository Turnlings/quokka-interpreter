#include "token.h"

int is_operator(TokenType type) {
    return type == OP_ADD || type == OP_SUB || type == OP_MUL || type == OP_DIV ||
           type == OP_GT || type == OP_GTE || type == OP_LT || type == OP_LTE ||
           type == OP_EQ || type == OP_DOT || type == OP_ADD_EQUALS || 
           type == OP_SUB_EQUALS || type == OP_MUL_EQUALS || type == OP_DIV_EQUALS;
}

int is_compound_assignment_operator(TokenType type) {
    return type == OP_ADD_EQUALS || type == OP_SUB_EQUALS || 
           type == OP_MUL_EQUALS || type == OP_DIV_EQUALS;
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

const char *token_type_to_string(TokenType type) {
    switch (type) {
        case PROGRAM: return "PRGRM";
        case STATEMENT_LIST: return "STMNT";
        case CLASS: return "CLASS";
        case FUNCTION: return "FUNC";
        case WHILE: return "WHILE";
        default: return "?";
    }
}

void print_binary_operator(ParseNode *node) {
    printf("(OP: ");
    switch(node->type) {
        case OP_ADD: printf("+"); break;
        case OP_SUB: printf("-"); break;
        case OP_MUL: printf("*"); break;
        case OP_DIV: printf("/"); break;
        case OP_DOT: printf("."); break;
        case OP_GT:  printf(">"); break;
        case OP_GTE: printf(">="); break; 
        case OP_LT:  printf("<"); break;
        case OP_LTE: printf("<="); break;
        case OP_EQ:  printf("=="); break;
        case ASSIGNMENT: printf("="); break;
    }
    printf(" ");
    print_ast(node->left);
    printf(",");
    print_ast(node->right);
    printf(") ");
}

void print_ast(ParseNode *node) {
    if (node == NULL) return;

    switch (node->type) {
        case OP_ADD: case OP_SUB: case OP_MUL: case OP_DIV: case OP_DOT:
        case OP_GT: case OP_GTE: case OP_LT: case OP_LTE: case OP_EQ:
        case ASSIGNMENT:
            print_binary_operator(node);
            break;
        case LITERAL: case IDENTIFIER:
            if (node->value.type == TYPE_INT) {
                printf("%d", node->value.data.intValue);
            } 
            else if (node->value.type == TYPE_FLOAT) {
                printf("%.2f", node->value.data.floatValue);
            } 
            else {
                printf("%s", node->value.data.stringValue);
            }
            break;
        case PROGRAM: case STATEMENT_LIST: case CLASS:
            printf("%s: ", token_type_to_string(node->type));
            print_ast(node->left);
            print_ast(node->right);
            break;
        case FUNCTION: case WHILE:
            printf("%s: ", token_type_to_string(node->type));
            print_ast(node->left);
            printf(" BODY: ");
            print_ast(node->right);
            break;
        case IF:
            printf("IF ");
            print_ast(node->left);
            printf("THEN ");
            print_ast(node->right->left);
            printf("ELSE");
            print_ast(node->right->right);
            break;
        case FOR:
            printf("FOR: ");
            print_ast(node->left->left);
            print_ast(node->left->right->left);
            print_ast(node->left->right->right);
            printf(" DO: ");
            print_ast(node->right);
            break;
        case OUT:
            printf("OUT: ");
            print_ast(node->left);
            break;
        default:
            printf("?");
            break;
    }
}

void free_ast(ParseNode *node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    free(node);
}