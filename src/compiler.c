#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "file_utils.h"
#include "token.h"
#include "lexer.h"
#include "transpiler.h"
#include "parser.h"

#define TOKEN_COUNT 128

void print_ast(ParseNode *node);

// TODO: make sure following naming conventions everywhere

int main() {
    char *input = read_file("quokka/test.qk");
    if (input) {
        // Tokenisation
        Token *tokens = tokenize(input, TOKEN_COUNT);
        if (tokens == NULL) {
            fprintf(stderr, "Tokenisation failed\n");
        }

        // Transpile to C from tokens to help debug tokenization
        char* c_code = transpileToC(tokens, TOKEN_COUNT);
        if (c_code == NULL) {
            fprintf(stderr, "Transpilation failed\n");
        }

        write_file("transpiled.c", c_code);

        // Parsing
        ParseNode *ast = parse_expression(tokens, TOKEN_COUNT);
        if (ast == NULL) {
            fprintf(stderr, "Parsing failed\n");
        }

        // Debug check parsing
        print_ast(ast);
        printf("\n");

        free(tokens);
        free(input);
        free(c_code);
        free(ast);
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
            printf("NUM: %d", node->data.intValue);
            break;
        case OPERATOR:
            printf("(%s", node->data.stringValue);
            print_ast(node->left);
            printf(",");
            print_ast(node->right);
            printf(")");
            break;
        case IDENTIFIER:
            printf("%s", node->data.stringValue);
            break;
        default:
            printf("?");
    }
}