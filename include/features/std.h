#ifndef STD_H
#define STD_H

#include "token.h"

Value *evaluate_std_lib_function(char *name, Value **args, int arg_c);

#endif