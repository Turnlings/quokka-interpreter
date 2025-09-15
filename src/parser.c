#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "token.h"

ParseNode *parse_expression();
ParseNode *parse_block();
ParseNode *add_child(ParseNode *parent, ParseNode *child);
ParseNode *create_node(TokenType type);
ParseNode *create_node_with_children(TokenType op, ParseNode *left, ParseNode *right);

Token* input_tokens;
int position;
Token current_t;
int count;

void syntax_error(char* string) {
    printf("\nSyntax Error: %s on line %d\n", string, input_tokens[position].line);
    if (position - 1 >= 0) {
        printf("%s ", input_tokens[position - 1].text);
    }
    if (position < count) {
        printf("_%s_ ", input_tokens[position].text);
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
        ParseNode *node = create_node(IDENTIFIER);
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
        ParseNode *node = create_node(LITERAL);
        node->value.type = TYPE_INT;
        node->value.data.intValue = atoi(current_t.text);
        advance();
        return node;
    } else if (match(FLOAT)) {
        ParseNode *node = create_node(LITERAL);
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
        ParseNode *node = create_node(LITERAL);
        node->value.type = TYPE_STRING;
        node->value.data.stringValue = current_t.text;
        advance();
        return node;
    } else if (match(TRUE) || match(FALSE)) {
        ParseNode *node = create_node(LITERAL);
        node->value.type = TYPE_BOOL;
        if (match(TRUE)) {
            node->value.data.intValue = 1;
        } else {
            node->value.data.intValue = 0;
        }
        advance();
        return node;
    } else {
        syntax_error("Expected Literal");
        return NULL;
    }
}

ParseNode *parse_term() {
    if (match(OP_NOT)) {
        advance();
        ParseNode *not = create_node(OP_NOT);
        not->left = parse_term();
        return not;
    }
    else if (match(IDENTIFIER)) {
        return parse_identifier();
    }
    return parse_literal();
}

ParseNode *parse_while() {
    expect(WHILE);
    ParseNode *condition = parse_expression();
    expect(DO);
    ParseNode *body = parse_block();
    ParseNode *node = create_node_with_children(WHILE, condition, body);
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

    ParseNode *control_c = create_node_with_children(CONTROL, condition, change);
    ParseNode *control_p = create_node_with_children(CONTROL, initialise, control_c);
    ParseNode *node = create_node_with_children(FOR, control_p, body);

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

    ParseNode *options = create_node_with_children(DO, body, else_block);
    ParseNode *if_statement = create_node_with_children(IF, condition, options);

    return if_statement;
}

ParseNode *parse_class_definition() {
    expect(CLASS);
    ParseNode *identifier = parse_identifier();
    ParseNode *body = parse_block();

    ParseNode *node = create_node_with_children(CLASS, identifier, body);
    return node;
}

ParseNode *parse_function_definition() {
    expect(DEF);
    ParseNode *identifier = parse_expression();
    expect(FUNCTION);
    ParseNode *body = parse_block();

    ParseNode *node = create_node_with_children(FUNCTION, identifier, body);
    return node;
}

ParseNode *parse_assignment() {
    if (match(ASSIGNMENT)) {
        ParseNode *node = create_node(ASSIGNMENT);
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
    TokenType operator_type = current_t.category - 2;
    advance();

    ParseNode* right = create_node(LITERAL);
    right->value.type = TYPE_INT;
    right->value.data.intValue = 1;

    ParseNode *identifier_copy = create_node(IDENTIFIER);
    identifier_copy->value.data.stringValue = strdup(identifier->value.data.stringValue);

    ParseNode *operator = create_node_with_children(operator_type, identifier_copy, right);
    ParseNode *assignment = create_node_with_children(ASSIGNMENT, identifier, operator);

    return assignment;
}

/**
 * @brief Converts input like x += 1 to x = x + 1
 */
ParseNode *parse_compound_assignment_operator() {
    ParseNode *identifier = parse_identifier();
    TokenType operator_type = current_t.category - 1;
    advance();
    ParseNode* right = parse_expression();

    ParseNode *identifier_copy = create_node(IDENTIFIER);
    identifier_copy->value.data.stringValue = identifier->value.data.stringValue;

    ParseNode *operator = create_node_with_children(operator_type, identifier_copy, right);
    ParseNode *assignment = create_node_with_children(ASSIGNMENT, identifier, operator);

    return assignment;
}

ParseNode *parse_multiply() {
    ParseNode *node = parse_term();

    while (match(OP_MUL) || match(OP_DIV)) {
        TokenType op = current_t.category;
        advance();
        ParseNode *right = parse_term();

        node = create_node_with_children(op, node, right);
    }
    return node;
}

ParseNode *parse_addition() {
    ParseNode *node = parse_multiply();

    while (match(OP_ADD) || match(OP_SUB)) {
        TokenType op = current_t.category;
        advance();
        ParseNode *right = parse_multiply();

        node = create_node_with_children(op, node, right);
    }
    return node;
}

ParseNode *parse_relation() {
    ParseNode *node = parse_addition();

    while (match(OP_EQ) || match(OP_NEQ) ||
           match(OP_LT) || match(OP_LTE) ||
           match(OP_GT) || match(OP_GTE)) {
        TokenType op = current_t.category;
        advance();
        ParseNode *right = parse_addition();

        node = create_node_with_children(op, node, right);
    }
    return node;
}

ParseNode *parse_logical_operators() {
    ParseNode *node = parse_relation();

    while (match(OP_AND) || match(OP_OR)) {
        TokenType op = current_t.category;
        advance();
        ParseNode *right = parse_relation();

        node = create_node_with_children(op, node, right);
    }
    return node;
}

ParseNode *parse_return() {
    expect(RETURN);
    ParseNode *node = create_node(RETURN);
    ParseNode* left = parse_expression();
    node->left = left;
    return node;
}

ParseNode *parse_out() {
    expect(OUT);
    ParseNode *node = create_node(OUT);
    ParseNode* left = parse_expression();
    node->left = left;
    return node;
}

ParseNode *parse_in() {
    expect(IN);
    return create_node(IN);
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

        ParseNode* set = create_node_with_children(SET, left, right);

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
    } else if (is_operator(peek().category)) { // All operators
        return parse_logical_operators();
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
        ParseNode *root = create_node(STATEMENT_LIST);
        while (!match(BRACES_R)) {
            ParseNode *node = create_node(STATEMENT_LIST);
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
    ParseNode *root = create_node(PROGRAM);

    while (position < count) {
        ParseNode *node = create_node(STATEMENT_LIST);
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

ParseNode *create_node(TokenType type) {
    ParseNode *node = parse_node_create(type);
    node->line = current_t.line;
    return node;
}

ParseNode *create_node_with_children(TokenType op, ParseNode *left, ParseNode *right) {
    ParseNode *new_node = create_node(op);
    new_node->left = left;
    new_node->right = right;
    return new_node;
}