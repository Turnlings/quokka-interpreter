#ifndef PARSER_H
#define PARSER_H

ParseNode* parse_statements(Token* tokens, int count);
ParseNode *parse_expression(Token *tokens, int count);

#endif