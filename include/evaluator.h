#include "token.h"
#ifndef EVALUATOR_H
#define EVALUATOR_H

int evaluate(ParseNode *node, HashTable *symbolTable);
int execute_function(ParseNode *node, Value *id_value);

#endif