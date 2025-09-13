#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "token.h"

ParseNode *parse_expression();
ParseNode *parse_block();
ParseNode *add_child(ParseNode *parent, ParseNode *child);

Token* input_tokens;
int position;
Token current_t;
int count;

void syntax_error(char* string) {
    printf("Syntax Error: %s at token %d\n", string, position);
    if (position - 1 >= 0) {
        printf("%s ", input_tokens[position - 1].text);
    }
    if (position < count) {
        printf("%s ", input_tokens[position].text);
    }
    if (position + 1 < count) {
        printf("%s ", input_tokens[position + 1].text);
    }

    printf("\n");
}

static void advance() {
    current_t = input_tokens[++position];
}

static Token peek() {
    return input_tokens[position + 1];
}

static bool peek_match(TokenType match) {
    return input_tokens[position + 1].category == match;
}

static void expect(TokenType expected) {
    if (current_t.category == expected) {
        advance();
    } else {
        syntax_error("Unexpected token type");
    }
}

static bool match(TokenType match) {
    return current_t.category == match;
}

ParseNode *parse_args() {
    ParseNode* root = NULL;
    expect(PAREN_L);
    while (!match(PAREN_R)) {
        ParseNode* node = parse_expression();
        root = add_child(root, node);
        if (match(COMMA)) {
            advance();
        }
    }
    advance();
    return root;
}

ParseNode *parse_identifier() {
    if (match(IDENTIFIER)) {
        ParseNode *node = parse_node_create(IDENTIFIER);
        node->value.data.stringValue = current_t.text;
        advance();

        if (match(PAREN_L)) {
            ParseNode *args = parse_args();
            node->right = args;
        }
        return node;
    } else {
        syntax_error("Expected Idnetifier");
        return NULL;
    }
}

ParseNode *parse_literal() {
    if (match(LITERAL)) {
        ParseNode *node = parse_node_create(LITERAL);
        node->value.type = TYPE_INT;
        node->value.data.intValue = atoi(current_t.text);
        advance();
        return node;
    } else if (match(FLOAT)) {
        ParseNode *node = parse_node_create(LITERAL);
        node->value.type = TYPE_FLOAT;
        char *end;
        double d = strtod(current_t.text, &end);
        if (end == current_t.text) {
            syntax_error("Invalid literal");
        }
        node->value.data.floatValue = d;
        advance();
        return node;
    } else if (match(STRING)) {
        ParseNode *node = parse_node_create(LITERAL);
        node->value.type = TYPE_STRING;
        node->value.data.stringValue = current_t.text;
        advance();
        return node;
    } else {
        syntax_error("Expected Literal");
        return NULL;
    }
}

ParseNode *parse_term() {
    if (match(IDENTIFIER)) {
        return parse_identifier();
    }
    return parse_literal();
}

ParseNode *parse_while() {
    expect(WHILE);
    ParseNode *condition = parse_expression();
    expect(DO);
    ParseNode *body = parse_block();
    ParseNode *node = parse_node_create(WHILE);
    node->left = condition;
    node->right = body;
    return node;
}

ParseNode *parse_for() {
    expect(FOR);
    ParseNode *initialise = parse_expression();
    expect(SEPERATOR);
    ParseNode *condition = parse_expression();
    expect(SEPERATOR);
    ParseNode *change = parse_expression();
    expect(SEPERATOR);
    ParseNode *body = parse_block();

    ParseNode *node = parse_node_create(FOR);
    ParseNode *control_p = parse_node_create(CONTROL);
    ParseNode *control_c = parse_node_create(CONTROL);

    node->left = control_p;
    control_p->left = initialise;
    control_p->right = control_c;
    control_c->left = condition;
    control_c->right = change;

    node->right = body;
    return node;
}

ParseNode *parse_if() {
    expect(IF);
    ParseNode *condition = parse_expression();
    expect(DO);
    ParseNode *body = parse_block();
    ParseNode *else_block = NULL;

    if (match(ELSE)) {
        expect(ELSE);
        else_block = parse_block();
    }

    ParseNode *options = parse_node_create(DO);
    options->left = body;
    options->right = else_block;

    ParseNode *if_statement = parse_node_create(IF);
    if_statement->left = condition;
    if_statement->right = options;

    return if_statement;
}

ParseNode *parse_class_definition() {
    expect(CLASS);
    ParseNode *identifier = parse_identifier();
    ParseNode *body = parse_block();

    ParseNode *node = parse_node_create(CLASS);
    node->left = identifier;
    node->right = body;
    return node;
}

ParseNode *parse_function_definition() {
    expect(DEF);
    ParseNode *identifier = parse_expression();
    expect(FUNCTION);
    ParseNode *body = parse_block();

    ParseNode *node = parse_node_create(FUNCTION);
    node->left = identifier;
    node->right = body;
    return node;
}

ParseNode *parse_assignment() {
    if (match(ASSIGNMENT)) {
        ParseNode *node = parse_node_create(ASSIGNMENT);
        node->value.data.stringValue = strdup(current_t.text);
        advance();
        return node;
    }
}

/**
 * @brief Convert x++ to x+=1
 */
ParseNode *parse_increment_operator() {
    ParseNode* identifier = parse_identifier();
    ParseNode *assignment = parse_node_create(ASSIGNMENT);
    TokenType operator_type = current_t.category - 2;
    ParseNode *operator = parse_node_create(operator_type);
    advance();

    ParseNode* right = parse_node_create(LITERAL);
    right->value.type = TYPE_INT;
    right->value.data.intValue = 1;

    ParseNode *identifier_copy = parse_node_create(IDENTIFIER);
    identifier_copy->value.data.stringValue = identifier->value.data.stringValue;

    assignment->left = identifier;
    assignment->right = operator;
    operator->left = identifier_copy;
    operator->right = right;

    return assignment;
}

/**
 * @brief Converts input like x += 1 to x = x + 1
 */
ParseNode *parse_compound_assignment_operator() {
    ParseNode* identifier = parse_identifier();
    ParseNode *assignment = parse_node_create(ASSIGNMENT);
    TokenType operator_type = current_t.category - 1;
    ParseNode *operator = parse_node_create(operator_type);
    advance();
    ParseNode* right = parse_expression();

    ParseNode *identifier_copy = parse_node_create(IDENTIFIER);
    identifier_copy->value.data.stringValue = identifier->value.data.stringValue;

    assignment->left = identifier;
    assignment->right = operator;
    operator->left = identifier_copy;
    operator->right = right;

    return assignment;
}

ParseNode *parse_operator() {
    if (is_operator(current_t.category)) {
        ParseNode *node = parse_node_create(current_t.category);
        node->value.data.stringValue = current_t.text;
        advance();
        return node;
    } else {
        syntax_error("Expected Operator");
        return NULL;
    }
}

ParseNode *parse_return() {
    expect(RETURN);
    ParseNode *node = parse_node_create(RETURN);
    ParseNode* left = parse_expression();
    node->left = left;
    return node;
}

ParseNode *parse_out() {
    expect(OUT);
    ParseNode *node = parse_node_create(OUT);
    ParseNode* left = parse_expression();
    node->left = left;
    return node;
}

ParseNode *parse_in() {
    expect(IN);
    return parse_node_create(IN);
}

ParseNode *parse_expression() {
    if (match(DEF)) {
        return parse_function_definition();
    } else if (match(CLASS)) {
        return parse_class_definition();
    } else if (match(SET)) {
        expect(SET);
        ParseNode* left = parse_term();
        expect(ASSIGNMENT);
        ParseNode* right = parse_expression();

        ParseNode* set = parse_node_create(SET);
        set->left = left;
        set->right = right;

        return set;
    } else if (match(WHILE)) {
        return parse_while();
    } else if (match(FOR)) {
        return parse_for();    
    }else if (match(IF)) {
        return parse_if();
    } else if (peek().category == ASSIGNMENT) {
        ParseNode* left = parse_term();
        ParseNode* assignment = parse_assignment();
        ParseNode* right = parse_expression();

        assignment->left = left;
        assignment->right = right;

        return assignment;
    } else if (match(OUT)) {
        return parse_out();
    } else if (match(IN)) {
        return parse_in();
    } else if (peek_match(OP_ADD_ADD) || peek_match(OP_SUB_SUB)) {
        return parse_increment_operator();
    } else if (is_compound_assignment_operator(peek().category)) {
        return parse_compound_assignment_operator();
    } else if (is_operator(peek().category)) { // Operator
        ParseNode* left = parse_term();
        ParseNode* operator = parse_operator();
        ParseNode* right = parse_expression();

        operator->left = left;
        operator->right = right;

        return operator;
    } else if (match(RETURN)) {
        return parse_return();
    } else {
        return parse_term();
    }
}

ParseNode *add_child(ParseNode *parent, ParseNode *child) {
    if (parent == NULL) return child;  // return new root

    ParseNode *cur = parent;
    while (cur->right != NULL) {
        cur = cur->right;
    }
    cur->right = child;
    return parent;
}

ParseNode *parse_block() {
    if(match(BRACES_L)) { // TODO: can this replace parts of parse_program?
        expect(BRACES_L);
        ParseNode *root = parse_node_create(STATEMENT_LIST);
        while (!match(BRACES_R)) {
            ParseNode *node = parse_node_create(STATEMENT_LIST);
            ParseNode *expr = parse_expression();
            node->left = expr;
            root = add_child(root, node);

            if (match(SEPERATOR)) {
                advance();
            } else {
                syntax_error("Missing semi-colon in block");
            }
        }
        advance();
        return root;
    } else {
        return parse_expression();
    }
}

ParseNode *parse_program() {
    ParseNode *root = parse_node_create(PROGRAM);

    while (position < count) {
        ParseNode *node = parse_node_create(STATEMENT_LIST);
        ParseNode *expr = parse_expression();
        node->left = expr;
        root = add_child(root, node);

        if (current_t.category == SEPERATOR) {
            advance();
        } else {
            syntax_error("Missing semi-colon");
        }
    }

    return root;
}

ParseNode *parse(Token *input, int size) {
    input_tokens = input;
    count = size;
    position = 0;
    current_t = input_tokens[position];

    return parse_program();
}