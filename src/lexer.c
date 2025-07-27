#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "file_utils.h"
#include "token.h"

char* substring(const char *input, int left, int right) {
    int length = right - left + 1;
    char *sub = malloc(length + 1);
    if (!sub) return NULL;
    strncpy(sub, input + left, length);
    sub[length] = '\0';
    return sub;
}

bool isAssignment(char *s) {
    return strcmp(s, "<-") == 0;
}

bool isOperator(char *s) {
    return !(strcmp(s, "+") && strcmp(s, "-") && strcmp(s, "*") 
          && strcmp(s, "/") && strcmp(s, "<") && strcmp(s, "<"));
}

bool isSeperator(char *s) {
    return strcmp(s, ";") == 0;
}

bool isWhitespace(char *s) {
    return !(strcmp(s, " ") && strcmp(s, "\n"));
}

bool isLiteral(char *s) {
    for (int i = 0; i < strlen(s); i++) {
        if (!isdigit(s[i])) {
            return false;
        }
    }
    return true; // No character was not a digit
}

Token* tokenize(char *input, int *token_count) {
    int left = 0, right = 0;
    int len = strlen(input);

    int n = 0;
    Token* tokens = malloc(*token_count * sizeof(Token)); // TODO: make dynamic
    if (tokens == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    while (right <= len && left <= right) {
        char* s = substring(input, left, right);
        assert(s != NULL);

        bool token_found = false;

        if (isAssignment(s)) {
            tokens[n].category = ASSIGNMENT;
            tokens[n].text = s;
            n++;
            right++;
            left = right;
            continue;
        } else if (isOperator(s)) {
            tokens[n].category = OPERATOR;
            char* next = substring(input,left,right+1);
            if (isOperator(next)) {
                tokens[n].text = next;
                n++;
                right += 2;
                left = right; 
                free(s);
                continue;
            } else {
                token_found = true;
                free(next);
            }
        } else if (isSeperator(s)) {
            tokens[n].category = SEPERATOR;
            token_found = true;
        } else if (isLiteral(s)) { // TODO: only works for single digits
            tokens[n].category = LITERAL;
            token_found = true;
        } else if (isWhitespace(s)) {
            left = right;
            left ++;
        } else {
            char* next = substring(input,right+1,right+1);
            if (isOperator(next) || isSeperator(next) || isWhitespace(next) || isAssignment(next)) {
                tokens[n].category = IDENTIFIER;
                token_found = true;
                free(next);
            }
        }

        right++;

        if (token_found) {
            tokens[n].text = s;
            n++;
            left = right;
        } else {
            free(s);
        }
    }

    // Update token count
    *token_count = n;

    return tokens;
}

// int main() {
//     char *input = readFile("test.tu");
//     if (input) {
//         Token* tokens = tokenize(input);

//         for (int i = 0; i < MAX_TOKENS; i++) {
//             printf("Token %d: category=%d, text='%s'\n", i, tokens[i].category, tokens[i].text);

//             //if (tokens[i].text == NULL) { break; } // Intentionally after so 1 invalid gets printed
//         }

//         free(tokens);
//         free(input);
//     } else {
//         fprintf(stderr, "File read failed\n");
//     }
//     return 0;
// }