#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "utils/file_utils.h"
#include "utils/hash_table.h"
#include "token.h"
#include "lexer.h"
#include "transpiler.h"
#include "parser.h"
#include "evaluator.h"

#define MAX_TOKEN_COUNT 128
#define MAX_SYMBOL_COUNT 128

void print_ast(ParseNode *node);

// TODO: make sure following naming conventions everywhere

int main() {
    char *input = read_file("quokka/test.qk");
    if (input) {
        // Tokenisation
        int token_count = MAX_TOKEN_COUNT;
        Token *tokens = tokenize(input, &token_count);
        if (tokens == NULL) {
            fprintf(stderr, "Tokenisation failed\n");
        }

        printf("Token Count: %d\n", token_count);

        // Transpile to C from tokens to help debug tokenization
        char* c_code = transpileToC(tokens, token_count);
        if (c_code == NULL) {
            fprintf(stderr, "Transpilation failed\n");
        }

        write_file("transpiled.c", c_code);

        // Parsing
        ParseNode *ast = parse(tokens, token_count);
        if (ast == NULL) {
            fprintf(stderr, "Parsing failed\n");
        }

        // Debug check parsing
        print_ast(ast);
        printf("\n");

        Value *return_value = evaluate(ast);
        if (return_value == NULL) {
            fprintf(stderr, "Evaluation failed\n");
        }

        if (return_value->type == TYPE_INT) {
            printf("Evaluation Return Value: %d\n", return_value->data.intValue);
        }
        else if (return_value->type == TYPE_STRING) {
            printf("Evaluation Return Value: %s\n", return_value->data.stringValue);
        }
        free_tokens(tokens, token_count);
        free(input);
        free(c_code);
        free_ast(ast);
    } else {
        fprintf(stderr, "File read failed\n");
    }
    return 0;
}

// For debugging the parser
void print_ast(ParseNode *node) {
    if (node == NULL) return;

    switch (node->type) {
        case LITERAL:
            printf("NUM: %d", node->value.data.intValue);
            //print_ast(node->right);
            break;
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_DOT:
            printf("(%s", node->value.data.stringValue);
            print_ast(node->left);
            printf(",");
            print_ast(node->right);
            printf(")");
            break;
        case OP_GT:
        case OP_GTE:
        case OP_LT:
        case OP_LTE:
        case OP_EQ:
            printf("( COMP OP: %s ", node->value.data.stringValue);
            print_ast(node->left);
            printf(",");
            print_ast(node->right);
            printf(")");
            break;
        case IDENTIFIER:
            printf("%s", node->value.data.stringValue);
            print_ast(node->right);
            break;
        case ASSIGNMENT:
            printf(" = ");
            print_ast(node->left);
            printf(",");
            print_ast(node->right);
            break;
        case PROGRAM:
        case STATEMENT_LIST:
            printf("STMT:");
            print_ast(node->left);
            print_ast(node->right);
            break;
        case TERN_IF:
        case IF:
            printf("IF ");
            print_ast(node->left);
            printf("THEN ");
            print_ast(node->right->left);
            printf("ELSE");
            print_ast(node->right->right);
            break;
        case FUNCTION:
            printf("FUNC: ");
            print_ast(node->left);
            printf(" BODY: ");
            print_ast(node->right);
            break;
        case WHILE:
            printf("WHILE: ");
            print_ast(node->left);
            printf(" DO: ");
            print_ast(node->right);
            break;
        case OUT:
            printf("OUT: ");
            print_ast(node->left);
            break;
        case CLASS:
            printf("CLASS: ");
            print_ast(node->left);
            print_ast(node->right);
            break;
        default:
            printf("?");
            break;
    }
}