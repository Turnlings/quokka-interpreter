#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "parser.h"

ParseNode *base_cases(Token *tokens);
ParseNode *parse_args(Token *tokens, int count);

ParseNode* parse_statements(Token* tokens, int count) {
    if (count <= 0) return NULL;
    int start = 0;
    ParseNode* list = NULL;
    ParseNode* last = NULL;
    for (int i = 0; i < count; i++) {
        if (tokens[i].category == SEPERATOR) {
            int stmt_len = i - start;
            if (stmt_len > 0) {
                ParseNode* stmt = parse_expression(tokens + start, stmt_len);
                ParseNode* node = parse_node_create(STATEMENT_LIST);
                node->left = stmt;
                node->right = NULL;
                if (!list) {
                    list = node;
                } else {
                    last->right = node;
                }
                last = node;
            }
            start = i + 1;
        }
    }

    return list;
}

/**
 * parse_expression - converts array of tokens into a parse tree 
 * 
 * @param tokens The array of tokens that has already passed through the lexer
 * @param count  The length of the array
 * 
 * @return The root of the parse tree
 */
ParseNode *parse_expression(Token *tokens, int count) {
    if (count == 0) {
        fprintf(stderr, "Passed an empty token to parse_expression\n");
        return NULL;
    }
    if (count == 1) {
        return base_cases(tokens);
    }
    printf("Expression token count: %d\n", count);

    // Function call: identifier followed by (args)
    // Count is > 1 otherwise would have already returned
    // TODO: there is probably a better way of handling
    if (tokens[0].category == IDENTIFIER && tokens[1].category == PAREN_L) {
        ParseNode *node = parse_node_create(IDENTIFIER);
        node->value.data.stringValue = tokens[0].text;

        // find matching ')'
        int depth = 1;
        int i;
        for (i = 2; i < count; i++) {
            if (tokens[i].category == PAREN_L) depth++;
            else if (tokens[i].category == PAREN_R) {
                depth--;
                if (depth == 0) break;  // found the matching one
            }
        }

        if (depth != 0) {
            fprintf(stderr, "Unmatched parenthesis in function call\n");
            return NULL;
        }

        // parse args between ( and )
        int args_len = i - 2;
        if (args_len > 0) {
            node->right = parse_args(tokens + 2, args_len);
        } else {
            node->right = NULL; // no args
        }

        // if there's more stuff after the function call, parse it too
        if (i + 1 < count) {
            ParseNode *parent = parse_expression(tokens + i + 1, count - (i + 1));
            parent->left = node;
            return parent;
        }

        return node;
    }

    // Next if parenthesis parse insides
    if (tokens[0].category == PAREN_L) {
        printf("HERE1\n");
        int depth = 1;
        for (int i = 1; i < count; i++) {
            printf("I: %d\n",i);
            if (tokens[i].category == PAREN_L) depth++;
            else if (tokens[i].category == PAREN_R) depth--;
            if (depth == 0) {
                printf("HERE2\n");
                // Everthing up to i goes in a statement list
                ParseNode *node = parse_expression(tokens + 1, i - 1);
            
                // If nothing to the right then can just return
                if (i == count - 1) {

                    return node;
                }

                // Otherwise, the right is the parent so return that
                ParseNode *parent = parse_expression(tokens + i + 1, count - i - 1);
                parent->left = node; // In theory the left should have failed to evaluate
                return parent;
            }
        }
    }

    // Assignment: right-associative, so parse from right to left
    for (int i = count - 1; i >= 0; i--) {
        if (tokens[i].category == ASSIGNMENT) {
            ParseNode *node = parse_node_create(ASSIGNMENT);
            node->value.data.stringValue = strdup(tokens[i].text);
            node->left = parse_expression(tokens, i);
            node->right = parse_expression(tokens + i + 1, count - i - 1);
            return node;
        }
    }

    for (int i = 0; i < count; i++) {
        TokenType type = tokens[i].category;
        switch (type) {
            ParseNode *node;
            case FUNCTION:
                node = parse_node_create(FUNCTION);
                node->value.data.stringValue = strdup(tokens[i].text);
                node->left = parse_expression(tokens, i);
                node->right = parse_expression(tokens + i + 1, count - i - 1);
                return node;
            case TERN_IF:
                node = parse_node_create(TERN_IF);
                node->value.data.stringValue = strdup(tokens[i].text);
                node->left = parse_expression(tokens, i);
                node->right = parse_expression(tokens + i + 1, count - i - 1);
                return node;
            case TERN_COLON:
                // Then first, which points to else
                node = parse_node_create(TERN_COLON);
                node->left = parse_expression(tokens, i);
                node->right = parse_expression(tokens + i + 1, count - i - 1);
                return node;
        }
    }

    for (int i = 0; i < count; i++) {
        TokenType type = tokens[i].category;

        if (is_operator(type)) {
            ParseNode *node = NULL;
            node = parse_node_create(type);
            node->value.data.stringValue = tokens[i].text;
            node->left = parse_expression(tokens, i);
            node->right = parse_expression(tokens + i + 1, count - i - 1);
            return node;
        }
    }

    return NULL;
}

int get_precedence(TokenType category) {
    switch(category) {
        case OP_ADD:
        case OP_SUB: return 1;
        case OP_MUL:
        case OP_DIV: return 2;
        default: return 0;
    }
}

ParseNode *base_cases(Token *tokens) {
    if (tokens[0].category == LITERAL) {
        ParseNode *node = parse_node_create(LITERAL);
        node->value.data.intValue = atoi(tokens[0].text);
        return node;
    }
    if (tokens[0].category == IDENTIFIER) {
        ParseNode *node = parse_node_create(IDENTIFIER);
        node->value.data.stringValue = tokens[0].text;
        return node;
    }

    return NULL; // Invalid!!!
}

ParseNode *append_child(ParseNode *parent, ParseNode *child);

ParseNode *parse_args(Token *tokens, int count) {
    printf("Parsing Args\n");

    if (count < 0) {
        fprintf(stderr, "Invalid function call args\n");
        return NULL;
    }

    ParseNode *root = NULL;
    int start = 0;
    int depth = 0;

    for (int i = 0; i < count; i++) {
        if (tokens[i].category == PAREN_L) depth++;
        else if (tokens[i].category == PAREN_R) depth--;
        else if (tokens[i].category == COMMA && depth == 0) {
            printf("Parsing arg %d (Count: %d)\n", i, i - start);
            ParseNode *expr = parse_expression(tokens + start, i - start);
            root = append_child(root, expr);
            start = i + 1;
        }
    }

    // Last args
    if (start < count) {
        printf("Parsing Final Arg (Count: %d)\n", count - start);
        ParseNode *expr = parse_expression(tokens + start, count - start);
        root = append_child(root, expr);
    }

    return root;
}

// Simple list appending to make code cleaner,
// TODO: build list struct to improve performance
ParseNode *append_child(ParseNode *parent, ParseNode *child) {
    if (parent == NULL) return child;  // return new root

    ParseNode *cur = parent;
    while (cur->right != NULL) {
        cur = cur->right;
    }
    cur->right = child;
    return parent;
}