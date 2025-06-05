#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "file_utils.h"

#define MAX_TOKENS 12

// Token can be identifier, keyword, seperator, operator, literal, comment or whitesepace
#define IDENTIFIER 0
#define KEYWORD 1
#define SEPERATOR 2
#define OPERATOR 3
#define LITERAL 4
#define COMMENT 5
#define WHITESPACE 6

typedef struct {
    char category;
    char *text;
} Token;

char* substring(const char *input, int left, int right) {
    int length = right - left + 1;
    char *sub = malloc(length + 1);
    if (!sub) return NULL;
    strncpy(sub, input + left, length);
    sub[length] = '\0';
    return sub;
}

bool isOperator(char *s) {
    return !(strcmp(s, "+") && strcmp(s, "-") && strcmp(s, "*") && strcmp(s, "/") && strcmp(s, "="));
}

bool isSeperator(char *s) {
    return strcmp(s, ";") == 0;
}

bool isWhitespace(char *s) {
    return !(strcmp(s, " ") && strcmp(s, "\n"));
}

Token* tokenize(char *input) {
    int left = 0, right = 0;
    int len = strlen(input);

    int n = 0;
    Token* tokens = malloc(MAX_TOKENS* sizeof(Token)); // TODO: make dynamic
    if (tokens == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    while (right <= len && left <= right) {
        char* s = substring(input, left, right);
        assert(s != NULL);

        bool token_found = false;

        if (isOperator(s)) {
            tokens[n].category = OPERATOR;
            token_found = true;
        } else if (isSeperator(s)) {
            tokens[n].category = SEPERATOR;
            token_found = true;
        } else if (isWhitespace(s)) {
            left = right;
            left ++;
        } else {
            char* next = substring(input,right+1,right+1);
            if (isOperator(next) || isSeperator(next) || isWhitespace(next)) {
                tokens[n].category = IDENTIFIER;
                token_found = true;
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

    return tokens;
}

int main() {
    char *input = readFile("test.tu");
    if (input) {
        Token* tokens = tokenize(input);

        for (int i = 0; i < MAX_TOKENS; i++) {
            printf("Token %d: category=%d, text='%s'\n", i, tokens[i].category, tokens[i].text);

            //if (tokens[i].text == NULL) { break; } // Intentionally after so 1 invalid gets printed
        }

        free(tokens);
        free(input);
    } else {
        fprintf(stderr, "File read failed\n");
    }
    return 0;
}