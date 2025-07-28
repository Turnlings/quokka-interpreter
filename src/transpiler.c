#include "token.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_VARIABLES 128

bool variableInUse(char* v, char* variables[]) {
    for (int i = 0; i < MAX_VARIABLES && variables[i] != NULL; i++) {
        if (strcmp(v, variables[i]) == 0) {
            return true;
        }
    }
    return false;
}

char* transpileToC(Token *tokens, int token_count) {
    if (!tokens) {return NULL;}

    char* output = malloc(MAX_BUFFER_SIZE);
    if (!output) {return NULL;}

    int v = 0;
    char *variables[MAX_VARIABLES] = {0}; 

    // TODO: keep track of already used variables, mostly as first time is declaration so needs a data type

    strcpy(output, "// Begin Program\nint main() {");

    int i = 0;
    while (i < token_count) {
        if (tokens[i].text == NULL) {break;} // TODO: add some sort of debug print?

        if (tokens[i].category == IDENTIFIER) {
            if (!variableInUse(tokens[i].text, variables)) {
                variables[v] = tokens[i].text;
                v++;
                strcat(output, "int ");
            }
            strcat(output, tokens[i].text);
        } else if (is_operator(tokens[i].category)) {
            if (strcmp(tokens[i].text, "<-") == 0) {
                strcat(output, "=");
            } else {
                strcat(output, tokens[i].text);
            }
        } else {
            strcat(output, tokens[i].text);
        }
        i++;
    }


    strcat(output, "\nreturn 0;}");

    return output;
}