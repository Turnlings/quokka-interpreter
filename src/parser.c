#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "token.h"

ParseNode *parse_expression();
ParseNode *parse_block();
ParseNode *parse_literal();
ParseNode *parse_term();
ParseNode *parse_increment_operator();
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

static Token peek_past() {
    int i = position + 1;
    int depth = 0;

    while (i < count) {
        Token token = input_tokens[i];

        if (token.category == SQUARE_L || token.category == PAREN_L) {
            depth++;
        } else if ((token.category == SQUARE_R || token.category == PAREN_R) && depth > 0) {
            depth--;
        } else if (depth == 0) {
            return token;
        }

        i++;
    }

    // Somethings gone wrong
    Token t = {0};
    t.category = NONE;
    return t;
}

static bool peek_match(TokenType match) {
    return input_tokens[position + 1].category == match;
}

static bool previous_match(TokenType match) {
    if (position - 1 < 0) return false;
    return input_tokens[position - 1].category == match;
}

static void expect(TokenType expected) {
    if (current_t.category == expected) {
        advance();
    } else {
        syntax_error("Unexpected token type");
    }
}

static void allow(TokenType allowed) {
    if (current_t.category == allowed) {
        advance();
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

        while (true) {
            if (match(PAREN_L)) {
                ParseNode *args = parse_args();
                node->right = args;
            } else if (match(SQUARE_L)) { // List access
                expect(SQUARE_L);
                ParseNode *index = parse_expression();
                expect(SQUARE_R);
                node = create_node_with_children(OP_INDEX, node, index);
            } else {
                break;
            }
        }
        return node;
    } else {
        syntax_error("Expected Identifier");
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
    if (peek_match(OP_ADD_ADD) || peek_match(OP_SUB_SUB)) {
        return parse_increment_operator();
    }
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
    allow(DO);
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
    allow(SEPERATOR);
    allow(DO);
    ParseNode *body = parse_block();

    ParseNode *control_c = create_node_with_children(CONTROL, condition, change);
    ParseNode *control_p = create_node_with_children(CONTROL, initialise, control_c);
    ParseNode *node = create_node_with_children(FOR, control_p, body);

    return node;
}

ParseNode *parse_if() {
    expect(IF);
    ParseNode *condition = parse_expression();
    allow(DO);
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
    allow(DO);
    ParseNode *body = parse_block();

    ParseNode *node = create_node_with_children(FUNCTION, identifier, body);
    return node;
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

int op_precedence(TokenType token) {
    switch(token) {
        case OP_DOT:              return 5;
        case OP_MUL: case OP_DIV: 
        case OP_MOD:              return 4;
        case OP_ADD: case OP_SUB: return 3;
        case OP_EQ:  case OP_NEQ: 
        case OP_LT:  case OP_LTE:
        case OP_GT:  case OP_GTE: return 2;
        case OP_AND: case OP_OR:  return 1;
        default:                  return -1;
    }
}

ParseNode *parse_op_binary(int min_precedence) {
    ParseNode *node = parse_term();

    while (true) {
        int prec = op_precedence(current_t.category);
        if (prec < min_precedence) break;

        TokenType op = current_t.category;
        advance();

        ParseNode *right = parse_op_binary(prec + 1);

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

ParseNode *parse_list() {
    expect(SQUARE_L);
    ParseNode *list = create_node(LIST);

    while(!match(SQUARE_R)) {
        add_child(list, parse_expression());
        if (match(COMMA)) advance();
    }

    expect(SQUARE_R);

    return list;
}

ParseNode *parse_import() {
    expect(IMPORT);
    ParseNode *import = create_node(IMPORT);
    ParseNode *filename = parse_literal();
    import->left = filename;

    return import;
}


ParseNode *parse_set() {
    expect(SET);
    ParseNode* left = parse_term();
    expect(ASSIGNMENT);
    ParseNode* right = parse_expression();

    ParseNode* set = create_node_with_children(SET, left, right);

    return set;
}

ParseNode *parse_assignment() {
    ParseNode* left = parse_term();
    expect(ASSIGNMENT);
    ParseNode *assignment = create_node(ASSIGNMENT);
    assignment->value.data.stringValue = strdup(current_t.text);
    ParseNode* right = parse_expression();

    assignment->left = left;
    assignment->right = right;

    return assignment;
}

ParseNode *parse_expression() {
    switch (current_t.category) {
        case DEF:      return parse_function_definition();
        case IMPORT:   return parse_import();
        case CLASS:    return parse_class_definition();
        case SET:      return parse_set();
        case WHILE:    return parse_while();
        case FOR:      return parse_for();
        case IF:       return parse_if();
        case SQUARE_L: return parse_list();
        case IN:       return parse_in();
        case OUT:      return parse_out();
        case RETURN:   return parse_return();
    }

    if (peek_past().category == ASSIGNMENT) {
        return parse_assignment();
    } else if (is_compound_assignment_operator(peek().category)) {
        return parse_compound_assignment_operator();
    } else if (is_operator(peek_past().category)) {
        // All operators with precedence climbing
        return parse_op_binary(0);
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
    if(match(BRACES_L)) {
        expect(BRACES_L);
        ParseNode *root = create_node(STATEMENT_LIST);
        while (!match(BRACES_R)) {
            ParseNode *node = create_node(STATEMENT_LIST);
            ParseNode *expr = parse_expression();
            node->left = expr;
            root = add_child(root, node);

            if (match(SEPERATOR)) {
                advance();
            } else if (!previous_match(BRACES_R)) {
                syntax_error("Missing semi-colon");
            }
        }
        expect(BRACES_R);
        return root;
    } else {
        ParseNode *node = parse_expression();
        if (match(SEPERATOR)) {
            advance();
        } else if (!previous_match(BRACES_R)) {
            syntax_error("Missing semi-colon");
        }
        return node;
    }
}

ParseNode *parse_program() {
    ParseNode *root = create_node(PROGRAM);

    while (position < count) {
        ParseNode *node = create_node(STATEMENT_LIST);
        ParseNode *expr = parse_block();
        node->left = expr;
        root = add_child(root, node);
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