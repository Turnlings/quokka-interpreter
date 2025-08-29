#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include <stdbool.h>

Token* tokenize(char *input, int *token_count);


// Token creation
void add_token(TokenType type);
void add_token_string(TokenType type, char *text);

// Token scanners
void number(void);
void identifier(void);
void string(void);

// Character classification
bool isDigit(char c);
bool isAlpha(char c);
bool isAlphaNumeric(char c);

// General string manipulation
char* substring(const char *input, int left, int right);

TokenType check_keyword(const char *str);

#endif