#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "utils/file_utils.h"
#include "utils/hash_table.h"
#include "features/list.h"
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "evaluator.h"

#define MAX_TOKEN_COUNT 128
#define MAX_SYMBOL_COUNT 128

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Must provide filename\nOptional flags\n\t--debug: prints more information\n");
        exit(0);
    }

    char *filename = argv[1];
    int debug = 0;

    if (argc > 2 && strcmp(argv[2], "--debug") == 0) {
        debug = 1;
    }

    if (debug) printf("Running file: %s\n", filename);
    char *input = read_file(filename);

    if (input) {
        // Tokenisation
        int token_count = MAX_TOKEN_COUNT;
        Token *tokens = tokenize(input, &token_count);
        if (tokens != NULL) {
            if (debug) printf("Tokenisation Successful\n");
        } else {
            fprintf(stderr, "Tokenisation failed\n");
        }

        if (debug) printf("Token Count: %d\n", token_count);

        // Parsing
        ParseNode *ast = parse(tokens, token_count);
        if (ast != NULL) {
            if (debug) printf("\nParsing Successful\n");
        } else {
            fprintf(stderr, "\nParsing failed\n");
        }

        // Debug check parsing
        if (debug) {
            print_ast(ast);
            printf("\n");
        }

        Value *return_value = evaluate(ast);
        if (return_value == NULL) {
            fprintf(stderr, "Evaluation failed\n");
        }

        if (debug) {
            printf("\nEvaluation Return Value: ");
            print_value(return_value);
            printf("\n");
        }
        
        free_ast(ast);
        free_tokens(tokens, token_count);
        free(input);
    } else {
        fprintf(stderr, "File read failed\n");
    }
    return 0;
}