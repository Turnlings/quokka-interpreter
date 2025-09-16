#include <stdio.h>
#include <stdlib.h>
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

int main() {
    char *input = read_file("quokka/test.qk");
    if (input) {
        // Tokenisation
        int token_count = MAX_TOKEN_COUNT;
        Token *tokens = tokenize(input, &token_count);
        if (tokens != NULL) {
            printf("Tokenisation Successful\n");
        } else {
            fprintf(stderr, "Tokenisation failed\n");
        }

        printf("Token Count: %d\n", token_count);

        // Parsing
        ParseNode *ast = parse(tokens, token_count);
        if (ast != NULL) {
            printf("\nParsing Successful\n");
        } else {
            fprintf(stderr, "\nParsing failed\n");
        }

        // Debug check parsing
        print_ast(ast);
        printf("\n");

        Value *return_value = evaluate(ast);
        if (return_value == NULL) {
            fprintf(stderr, "Evaluation failed\n");
        }

        printf("\nEvaluation Return Value: ");
        print_value(return_value);
        printf("\n");
        
        free_ast(ast);
        free_tokens(tokens, token_count);
        free(input);
    } else {
        fprintf(stderr, "File read failed\n");
    }
    return 0;
}