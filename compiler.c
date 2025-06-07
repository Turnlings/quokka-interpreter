#include <stdio.h>
#include <stdlib.h>
#include "file_utils.h"
#include "token.h"
#include "lexer.h"
#include "transpiler.h"
#include <assert.h>

#define TOKEN_COUNT 128

int main() {
    char *input = readFile("test.qk");
    if (input) {
        Token* tokens = tokenize(input, TOKEN_COUNT);
        assert(tokens != NULL);

        char* c_code = transpileToC(tokens, TOKEN_COUNT);
        assert(c_code != NULL); 

        writeFile("transpiled.c", c_code);

        free(tokens);
        free(input);
    } else {
        fprintf(stderr, "File read failed\n");
    }
    return 0;
}