#include "token.h"
#ifndef EVALUATOR_H
#define EVALUATOR_H

Value *evaluate(ParseNode *node);
Value *execute_function(ParseNode *node, Value *id_value);
Value *build_object(ParseNode *node, Value *class);
Value *call_object(ParseNode *node);

#endif