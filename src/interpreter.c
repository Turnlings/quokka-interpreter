#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "utils/file_utils.h"
#include "utils/hash_table.h"
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
        if (tokens == NULL) {
            fprintf(stderr, "Tokenisation failed\n");
        }

        printf("Token Count: %d\n", token_count);

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
        
        free_ast(ast);
        free_tokens(tokens, token_count);
        free(input);
    } else {
        fprintf(stderr, "File read failed\n");
    }
    return 0;
}