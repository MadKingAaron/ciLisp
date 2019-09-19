//Aaron Jimenez
// © 2019
//ciLisp
//May 8, 2018
#ifndef __cilisp_h_
#define __cilisp_h_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "ciLispParser.h"

int yyparse(void);

int yylex(void);

void yyerror(char *);

typedef enum oper { // must be in sync with funcs in resolveFunc()
    NEG_OPER, // 0
    ABS_OPER, // 1
    EXP_OPER, // 2
    SQRT_OPER, // 3
    ADD_OPER, // 4
    SUB_OPER, // 5
    MULT_OPER, // 6
    DIV_OPER, // 7
    REMAINDER_OPER, // 8
    LOG_OPER, // 9
    POW_OPER, // 10
    MAX_OPER, // 11
    MIN_OPER, // 12
    EXP2_OPER, // 13
    CBRT_OPER, // 14
    HYPOT_OPER, // 15
    PRINT_OPER, //16
    READ_OPER, //17
    RAND_OPER, //18
    SMALLER_OPER, //19
    LARGER_OPER, //20
    EQUAL_OPER,//21
    CUSTOM_FUNC=255
} OPER_TYPE;

typedef enum dataType
{
    NO_TYPE,//0
    REAL_TYPE,//1
    INTEGER_TYPE//2
}DATA_TYPE;

OPER_TYPE resolveFunc(char *);

typedef enum {
    NUM_TYPE, FUNC_TYPE, SYM_TYPE, COND_TYPE
} AST_NODE_TYPE;

typedef enum
{
    VARIABLE_TYPE = 0,
    LAMBDA_TYPE,//1
    ARG_TYPE//2
}SYMBOL_TYPE;

typedef struct stack_node{
    struct ast_node *val;
    struct stack_node *next;
} STACK_NODE;

typedef struct {
    DATA_TYPE data_type;
    union{
        long intVal;
        double realVal;
    }value;

} NUMBER_AST_NODE;

typedef struct{
    char* name;
} SYMBOL_AST_NODE;

typedef struct symbol_table_node{
    SYMBOL_TYPE type;
    DATA_TYPE val_type;
    char *ident;
    struct ast_node *val;
    STACK_NODE* stack;
    struct symbol_table_node *next;
}SYMBOL_TABLE_NODE;

typedef struct return_value
{
    DATA_TYPE type;
    double value;
} RETURN_VALUE;

typedef struct {
    char *name;
    struct ast_node *opList;
} FUNCTION_AST_NODE;

typedef struct condition_ast_node{
    struct ast_node* condExpr;
    struct ast_node* trueExpr;
    struct ast_node* falseExpr;
} COND_AST_NODE;

//typedef struct {
//    struct condition_ast_node* cond;
//} COND_CONT_AST_NODE;

typedef struct ast_node {
    AST_NODE_TYPE type;
    SYMBOL_TABLE_NODE* symbolTable;
    struct ast_node *parent;
    union {
        COND_AST_NODE* cond;
        NUMBER_AST_NODE number;
        FUNCTION_AST_NODE function;
        SYMBOL_AST_NODE symbol;
    } data;
    struct ast_node *next;
} AST_NODE;

AST_NODE *number(DATA_TYPE data_type, double value);
//AST_NODE *realNumber(double value);

//Conditionals
AST_NODE* setConditional(AST_NODE* condExpr, AST_NODE* trueExpr, AST_NODE* falseExpr);

AST_NODE *function(char *funcName, AST_NODE *opList);
AST_NODE* addFunctionToOpList(AST_NODE* opToAdd, AST_NODE* opList);
void setParent(AST_NODE* opList, AST_NODE* parent);

//Free Nodes
void freeNode(AST_NODE *p);
void freeOpList(AST_NODE* opList);
void freeSymbolTableNodes(SYMBOL_TABLE_NODE* currentNode);

//Evaluation
RETURN_VALUE* eval(AST_NODE *ast);
bool tooManyOps(OPER_TYPE operType, AST_NODE* ast);
bool tooFewOps(OPER_TYPE operType, AST_NODE* opList);
RETURN_VALUE* addOps(AST_NODE* opList);
RETURN_VALUE* multOps(AST_NODE* opList);



SYMBOL_TABLE_NODE* createSymbolTable(char* symbol, AST_NODE* s_expr);
SYMBOL_TABLE_NODE* addSymbolToList(SYMBOL_TABLE_NODE *let_List, SYMBOL_TABLE_NODE * let_elem);
AST_NODE* setSymbolTable(SYMBOL_TABLE_NODE*let_section ,AST_NODE* s_expr);

SYMBOL_TABLE_NODE* createSymbol(DATA_TYPE type, char* symbolName, AST_NODE* s_expr);
AST_NODE* symbol(char* symbol);

SYMBOL_TABLE_NODE* findSymbol(SYMBOL_TABLE_NODE* symTable, SYMBOL_TABLE_NODE* sym);
SYMBOL_TABLE_NODE* searchSymbol(AST_NODE *p);
RETURN_VALUE* evalSymbol(AST_NODE* p);


//Lambda Expressions
SYMBOL_TABLE_NODE* createLambda(DATA_TYPE data_type, char* symbol, SYMBOL_TABLE_NODE* arg_list, AST_NODE* s_expr);
SYMBOL_TABLE_NODE* addSymbolToArgList(char* symbolToAdd, SYMBOL_TABLE_NODE* arg_list);

//Data type
bool strIsEqual(char* str1, char* str2);
bool checkIfSameDataType(RETURN_VALUE* op1, RETURN_VALUE* op2);

//Evaluating Lambdas
RETURN_VALUE* evalCustomFunc(AST_NODE* p);
SYMBOL_TABLE_NODE* findLambda(AST_NODE* p);

bool pushValueToStackLambda(AST_NODE* p, SYMBOL_TABLE_NODE* lambda);
bool pushToStack(RETURN_VALUE* toPush, SYMBOL_TABLE_NODE* currentArg);

RETURN_VALUE* popArgFromStack(SYMBOL_TABLE_NODE* symbolTableNode);


AST_NODE* symbol(char* symbol);



#endif
