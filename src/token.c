#include "token.h"
#include "features/list.h"
#include "utils/hash_table.h"

int is_operator(TokenType type) {
    return type == OP_ADD || type == OP_SUB || type == OP_MUL || type == OP_DIV || type == OP_MOD ||
           type == OP_GT || type == OP_GTE || type == OP_LT || type == OP_LTE ||
           type == OP_EQ || type == OP_NEQ || type == OP_DOT || type == OP_ADD_EQUALS || 
           type == OP_SUB_EQUALS || type == OP_MUL_EQUALS || type == OP_DIV_EQUALS || type == OP_MOD_EQUALS ||
           type == OP_AND || type == OP_OR;
}

int is_compound_assignment_operator(TokenType type) {
    return type == OP_ADD_EQUALS || type == OP_SUB_EQUALS || 
           type == OP_MUL_EQUALS || type == OP_DIV_EQUALS || 
           type == OP_MOD_EQUALS;
}

ParseNode *parse_node_create(TokenType type){
    ParseNode *node = malloc(sizeof(ParseNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    node->type = type;
    node->value.type = TYPE_NONE;

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

Value *value_create(ValueType type) {
    Value *value = malloc(sizeof(Value));
    value->type = type;
}

Value *value_copy(Value *old) {
    Value *copy = malloc(sizeof(Value));
    copy->type = old->type;

    switch (old->type) {
        case TYPE_INT:
            copy->data.intValue = old->data.intValue;
            break;
        case TYPE_FLOAT:
            copy->data.floatValue = old->data.floatValue;
            break;
        case TYPE_STRING:
            if (old->data.stringValue)
                copy->data.stringValue = strdup(old->data.stringValue);
            else
                copy->data.stringValue = NULL;
            break;
        case TYPE_LIST:
            List *list = list_create(old->data.list->array_length);
            list_copy(old->data.list, list,0);
            copy->data.list = list;
            break;
        default:
            fprintf(stderr, "Unknown ValueType in deep_copy_value\n");
            free(copy);
            return NULL;
    }
}

void value_destroy(Value value) {
    if(value.type == TYPE_OBJECT) {
        // Derefence self to stop infinite loop
        hashtable_set(value.data.object_fields, "self", value_create(TYPE_NONE));

        hashtable_destroy(value.data.object_fields);
        value.data.object_fields = NULL;
        value.type = TYPE_NONE;
    }
    if(value.type == TYPE_LIST) {
        list_destroy(value.data.list);
        value.data.list = NULL;
        value.type = TYPE_NONE;
    }
}

void print_value(Value *value) {
    if (value->type == TYPE_INT || value->type == TYPE_BOOL) {
        printf("%d", value->data.intValue);
    }
    else if (value->type == TYPE_FLOAT) {
        printf("%f", value->data.floatValue);
    }
    else if (value->type == TYPE_STRING) {
        printf("%s", value->data.stringValue);
    }
    else if (value->type == TYPE_OBJECT) {
        printf("OBJECT");
    }
    else if (value->type == TYPE_LIST) {
        printf("[");
        for (int i = 0; i <= value->data.list->tail; i++) {
            print_value(value->data.list->items[i]);
            if (i < value->data.list->tail) {
                printf(",");
            }
        }
        printf("]");
    } else {
        printf("?");
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
        case OP_NEQ: printf("!="); break;
        case OP_AND: printf("&&"); break;
        case OP_OR:  printf("||"); break;
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
        case OP_NEQ: case ASSIGNMENT: case OP_AND: case OP_OR:
            print_binary_operator(node);
            break;
        case LITERAL: case IDENTIFIER:
            if (node->value.type == TYPE_INT) {
                printf("%d", node->value.data.intValue);
            } 
            else if (node->value.type == TYPE_FLOAT) {
                printf("%.2f", node->value.data.floatValue);
            } 
            else if (node->value.type == TYPE_BOOL) {
                printf("%d", node->value.data.intValue);
            } 
            else if (node->value.data.stringValue != NULL){
                printf("%s", node->value.data.stringValue);
            }

            if (node->right!=NULL){
                printf(",");
                print_ast(node->right);
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
        case LIST:
            printf("[");
            print_ast(node->right);
            printf("]");
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