
#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Analysis.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "parser.h"

#define MAX_VARS 128

// Simple symbol table for local variables
typedef struct {
    char name[32];
    LLVMValueRef value;
} Symbol;

typedef struct {
    Symbol symbols[MAX_VARS];
    int count;
} SymbolTable;

static LLVMValueRef lookup_var(SymbolTable *table, const char *name) {
    for (int i = 0; i < table->count; ++i) {
        if (strcmp(table->symbols[i].name, name) == 0)
            return table->symbols[i].value;
    }
    return NULL;
}

static void add_var(SymbolTable *table, const char *name, LLVMValueRef value) {
    for (int i = 0; i < table->count; ++i) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            table->symbols[i].value = value;
            return;
        }
    }
    strncpy(table->symbols[table->count].name, name, 31);
    table->symbols[table->count].name[31] = '\0';
    table->symbols[table->count].value = value;
    table->count++;
}

/**
 * 
 */
// Recursively generate LLVM IR from AST
static LLVMValueRef llvm_from_node(LLVMBuilderRef builder, SymbolTable *table, ParseNode *node) {
    if (!node) return NULL;

    switch (node->type) {
        case LITERAL:
            return LLVMConstInt(LLVMInt32Type(), node->data.intValue, 0);
        case IDENTIFIER: {
            LLVMValueRef var = lookup_var(table, node->data.stringValue);
            if (!var) {
                fprintf(stderr, "Undefined variable: %s\n", node->data.stringValue);
                return NULL;
            }
            return LLVMBuildLoad2(builder, LLVMInt32Type(), var, node->data.stringValue);
        }
        case OPERATOR: {
            LLVMValueRef lhs = llvm_from_node(builder, table, node->left);
            LLVMValueRef rhs = llvm_from_node(builder, table, node->right);
            if (!lhs || !rhs) return NULL;
            if (strcmp(node->data.stringValue, "+") == 0)
                return LLVMBuildAdd(builder, lhs, rhs, "addtmp");
            if (strcmp(node->data.stringValue, "-") == 0)
                return LLVMBuildSub(builder, lhs, rhs, "subtmp");
            // Add more operators as needed
            return NULL;
        }
        case ASSIGNMENT: {
            // node->left: identifier, node->right: value
            if (!node->left || node->left->type != IDENTIFIER) {
                fprintf(stderr, "Invalid assignment\n");
                return NULL;
            }
            const char *varname = node->left->data.stringValue;
            LLVMValueRef value = llvm_from_node(builder, table, node->right);
            LLVMValueRef var = lookup_var(table, varname);
            if (!var) {
                // allocate variable
                var = LLVMBuildAlloca(builder, LLVMInt32Type(), varname);
                add_var(table, varname, var);
            }
            LLVMBuildStore(builder, value, var);
            return value;
        }
        case STATEMENT_LIST: {
            LLVMValueRef last = NULL;
            for (ParseNode *stmt = node->left; stmt; stmt = stmt->right) {
                last = llvm_from_node(builder, table, stmt->left);
            }
            return last;
        }
        default:
            return NULL;
    }
}

void generate_llvm_from_ast(ParseNode *root) {
    LLVMModuleRef mod = LLVMModuleCreateWithName("quokka_module");
    LLVMBuilderRef builder = LLVMCreateBuilder();

    // int main() { ... }
    LLVMTypeRef funcType = LLVMFunctionType(LLVMInt32Type(), NULL, 0, 0);
    LLVMValueRef func = LLVMAddFunction(mod, "main", funcType);
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(func, "entry");
    LLVMPositionBuilderAtEnd(builder, entry);

    SymbolTable table = {0};
    LLVMValueRef retVal = llvm_from_node(builder, &table, root);
    if (!retVal) retVal = LLVMConstInt(LLVMInt32Type(), 0, 0);
    LLVMBuildRet(builder, retVal);

    // Write IR to file
    char *error = NULL;
    LLVMPrintModuleToFile(mod, "output.ll", &error);
    if (error) {
        fprintf(stderr, "Error writing LLVM IR: %s\n", error);
        LLVMDisposeMessage(error);
    }

    // Optionally, print IR to stdout
    char *irstr = LLVMPrintModuleToString(mod);
    printf("%s\n", irstr);
    LLVMDisposeMessage(irstr);

    LLVMDisposeBuilder(builder);
    LLVMDisposeModule(mod);
}