#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "token.h"
#include "lexer.h"

int start = 0, current = 0;

Token *tokens;
int token_count;
char *source;

Token* tokenize(char *input, int *max_token_count) {
    source = input; 
    start = 0;
    current = 0;
    token_count = 0;

    tokens = malloc(*max_token_count * sizeof(Token));
    if (!tokens) return NULL;

    while (source[current] != '\0') {
        start = current; // mark start of token
        char c = advance();
        switch (c) {
            case '+': add_token(OP_ADD); break;
            case '-': add_token(match('>') ? RETURN : OP_SUB); break;
            case '*': add_token(OP_MUL); break;
            case '/': add_token(OP_DIV); break;
            case '=': add_token(ASSIGNMENT); break;
            case ';': add_token(SEPERATOR); break;
            default:
                if (isDigit(c)) {
                    number();
                } else if (isAlpha(c)) {
                    identifier();
                }
                break;
        }
    }

    *max_token_count = token_count;
    return tokens;
}

char peek() {
    return source[current];
}

void add_token(TokenType type) {
    tokens[token_count].category = type;
    tokens[token_count].text = substring(source, start, current - 1);
    token_count++;
}

void add_token_string(TokenType type, char* text) {
    tokens[token_count].category = type;
    tokens[token_count].text = text;
    token_count++;
}

char advance() {
    return source[current++];
}

bool match(char c) {
    if (c != source[current]) { return false; }

    current++;
    return true;
}

void number() {
    while (isDigit(peek())) advance();

    add_token_string(LITERAL, substring(source, start, current - 1));
}

void identifier() {
    while (isAlphaNumeric(peek())) advance();

    add_token_string(IDENTIFIER, substring(source, start, current - 1));
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

bool isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

char* substring(const char *input, int left, int right) {
    int length = right - left + 1;
    char *sub = malloc(length + 1);
    if (!sub) return NULL;
    strncpy(sub, input + left, length);
    sub[length] = '\0';
    return sub;
}