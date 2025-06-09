#include <stdio.h>
#include <stdlib.h>
#include "file_utils.h"
#include "token.h"
#include "lexer.h"
#include "transpiler.h"
#include <assert.h>

#define TOKEN_COUNT 128

// TODO: make sure following naming conventions everywhere

int main() {
    char *input = readFile("quokka/test.qk");
    if (input) {
        Token* tokens = tokenize(input, TOKEN_COUNT);
        if (tokens == NULL) {
            fprintf(stderr, "Tokenisation failed\n");
        }

        char* c_code = transpileToC(tokens, TOKEN_COUNT);
        if (c_code == NULL) {
            fprintf(stderr, "Transpilation failed\n");
        }

        writeFile("transpiled.c", c_code);

        free(tokens);
        free(input);
        free(c_code);
    } else {
        fprintf(stderr, "File read failed\n");
    }
    return 0;
}