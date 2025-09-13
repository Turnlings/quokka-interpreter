#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "token.h"
#include "lexer.h"

static char advance();
static char peek();
static bool match(char c);

int start = 0, current = 0;
int line = 1;

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
            case '+': add_token(match('=') ? OP_ADD_EQUALS : match('+') ? OP_ADD_ADD : OP_ADD); break;
            case '-': add_token(match('=') ? OP_SUB_EQUALS : match('-') ? OP_SUB_SUB : OP_SUB); break;
            case '*': add_token(match('=') ? OP_MUL_EQUALS : OP_MUL); break;
            case '/':     
                if (match('/')) {
                    // Just ignore whole line of comment
                    while (peek() != '\n' && peek() != '\0') advance();
                } else {
                    add_token(match('=') ? OP_DIV_EQUALS : OP_DIV);
                } 
                break;
            case '%': add_token(OP_MOD); break; 

            case '&':
                if (match('&')) {
                    add_token(OP_AND);
                } else {
                    syntax_error("Unexpected single &");
                }
                break;
            case '|':
                if (match('|')) {
                    add_token(OP_OR);
                } else {
                    syntax_error("Unexpected single |");
                }
                break;
            case '!': add_token(OP_NOT); break;

            case '>': add_token(match('=') ? OP_GTE : match('>') ? OUT : OP_GT); break;
            case '<': add_token(match('=') ? OP_LTE : match('<') ? IN : OP_LT); break;

            case '=': add_token(match('=') ? OP_EQ : match('>') ? FUNCTION : ASSIGNMENT); break;
            case '.': add_token(OP_DOT); break;
            case ';': add_token(SEPERATOR); break;
            case '?': add_token(TERN_IF); break;
            case ':': add_token(TERN_COLON); break;
            case '(': add_token(PAREN_L); break;
            case ')': add_token(PAREN_R); break;
            case '{': add_token(BRACES_L); break;
            case '}': add_token(BRACES_R); break;
            case ',': add_token(COMMA); break;
            case '"': string(); break;
            case '\n': line++; break;
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

static char peek() {
    return source[current];
}

void add_token(TokenType type) {
    tokens[token_count].category = type;
    tokens[token_count].text = substring(source, start, current - 1);
    tokens[token_count].line = line;
    token_count++;
}

void add_token_string(TokenType type, char* text) {
    tokens[token_count].category = type;
    tokens[token_count].text = text;
    tokens[token_count].line = line;
    token_count++;
}

static char advance() {
    return source[current++];
}

static bool match(char c) {
    if (c != source[current]) { return false; }

    current++;
    return true;
}

void number() {
    TokenType type = LITERAL;
    while (isDigit(peek())) advance();

    if (match('.')) {
        type = FLOAT;
        while (isDigit(peek())) advance();
    }

    add_token_string(type, substring(source, start, current - 1));
}

void identifier() {
    while (isAlphaNumeric(peek())) advance();

    char *text = substring(source, start, current - 1);
    TokenType type = check_keyword(text);
    add_token_string(type, text);
}

void string() {
    while (peek()!='"') advance();

    char *text = substring(source, start + 1, current - 1);
    add_token_string(STRING, text);
    advance();
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

typedef struct {
    const char *name;
    TokenType type;
} Keyword;

Keyword keywords[] = {
    {"if", IF},
    {"else", ELSE},
    {"while", WHILE},
    {"for", FOR},
    {"do", DO},
    {"def", DEF},
    {"class", CLASS},
    {"set", SET},
    {"return", RETURN},
    {"true", TRUE},
    {"false", FALSE},
    {NULL, 0}
};

TokenType check_keyword(const char *str) {
    for (int i = 0; keywords[i].name != NULL; i++) {
        if (strcmp(str, keywords[i].name) == 0) {
            return keywords[i].type;
        }
    }
    return IDENTIFIER;
}

void free_tokens(Token *tokens, int count) {
    for (int i = 0; i < count; i++) {
        if (tokens[i].text) {
            free(tokens[i].text);
        }
    }
    free(tokens);
}