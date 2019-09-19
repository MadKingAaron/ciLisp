//Aaron Jimenez
// © 2019
//ciLisp
//May 8, 2018

#include "ciLisp.h"

void yyerror(char *s) {
    fprintf(stderr, "\nERROR: %s\n", s);
    // note stderr that normally defaults to stdout, but can be redirected: ./src 2> src.log
    // CLion will display stderr in a different color from stdin and stdout
}

//Utility to compare two strings and return boolean value

bool strIsEqual(char* str1, char* str2)
{
    return strcmp(str1, str2) == 0;
}



//
// find out which function it is
//

char *func[] = {"neg",//0
                "abs",//1
                "exp",//2
                "sqrt",//3
                "add",//4
                "sub",//5
                "mult",//6
                "div",//7
                "remainder",//8
                "log",//9
                "pow",//10
                "max",//11
                "min",//12
                "exp2",//13
                "cbrt",//14
                "hypot",//15
                "print",//16
                "read",//17
                "rand",//18
                "smaller",//19
                "larger",//20
                "equal",//21
                ""};

OPER_TYPE resolveFunc(char *funcName)
{
    int i = 0;
    while (func[i][0] != '\0')
    {
        if (strcmp(func[i], funcName) == 0)
            return i;

        i++;
    }
    return CUSTOM_FUNC;
}

//
// create a node for a number
//
AST_NODE *number(DATA_TYPE data_type, double value)
{
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(NUMBER_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = NUM_TYPE;
    p->data.number.data_type = data_type;
    switch (data_type)
    {
        case NO_TYPE:
        case REAL_TYPE:
            p->data.number.value.realVal = value;
            fprintf(stderr,"DEBUG:    %lf\n", p->data.number.value.realVal);


            break;
        case INTEGER_TYPE:
            p->data.number.value.intVal = (long)value;

            fprintf(stderr,"DEBUG:    %d\n", (int)p->data.number.value.intVal);
            break;
        default:
            yyerror("Something went wrong with number's data type");
            break;
    }

    return p;
}

//Add operation to opList
AST_NODE* addFunctionToOpList(AST_NODE* opToAdd, AST_NODE* opList)
{
    //If opList is empty return the opToAdd
    if(opList == NULL)
    {
        opToAdd->next = NULL;
        return opToAdd;
    }



    opToAdd ->next =  opList;
//    AST_NODE* currentNode = opList;
//    AST_NODE* previousNode;
//
//    //Find last element of opList
//    while(currentNode != NULL)
//    {
//        previousNode = currentNode;
//        currentNode = currentNode->next;
//    }
//
//    //Add opToAdd to end of opList
//    previousNode->next = opToAdd;



    return opToAdd;
}


//
// create a node for a function
//
AST_NODE *function(char *funcName, AST_NODE *op_List)
{
    AST_NODE *p;
    //size_t nodeSize;


    // allocate space for the fixed sie and the variable part (union)
    //nodeSize = sizeof(AST_NODE) + sizeof(FUNCTION_AST_NODE);
    if ((p = malloc(sizeof(AST_NODE))) == NULL)
        yyerror("out of memory");

    //Set parent of operations to p
    if(op_List != NULL)
    {
        setParent(op_List, p);
    }

    p->type = FUNC_TYPE;
    p->data.function.name = funcName;
    p->data.function.opList = op_List;


    return p;
}

void setParent(AST_NODE* opList, AST_NODE* parent)
{
    AST_NODE* currentNode = opList;

    while(currentNode != NULL)
    {
        currentNode->parent = parent;

        currentNode= currentNode->next;
    }

}

//
// free a node
//
void freeNode(AST_NODE *p)
{
    if (!p)
        return;

    //Free Symbol Table
//    if((p->symbolTable))
//    {
//       SYMBOL_TABLE_NODE* currentNode = p->symbolTable;
//       SYMBOL_TABLE_NODE* previousNode;
//
//
//
//       while(currentNode != NULL)
//       {
//           freeSymbolTableNodes(currentNode);
//           previousNode = currentNode;
//           fprintf(stderr, "%s\n",currentNode->ident);
//           currentNode = currentNode->next;
//           free(previousNode);
//       }
   // }

    if (p->type == FUNC_TYPE)
    {

        free(p->data.function.name);
        freeOpList(p->data.function.opList);
    }

    free(p);
}


void freeSymbolTableNodes(SYMBOL_TABLE_NODE* currentNode)
{
    STACK_NODE* currentStackNode;
    STACK_NODE* previousStackNode;
    switch (currentNode->type)
    {
        case VARIABLE_TYPE:
            freeNode(currentNode->val);
            free(currentNode->ident);
            break;
        case ARG_TYPE:
            currentStackNode = currentNode->stack;
            while(currentStackNode != NULL)
            {
                freeNode(currentStackNode->val);

                previousStackNode = currentStackNode;
                currentStackNode = currentStackNode->next;
                free(previousStackNode);
            }
            //char* temp = currentNode->ident;
            //free((temp));
            break;

        case LAMBDA_TYPE:
            freeNode(currentNode->val);
            free(currentNode->ident);
            break;

    }
}

void freeOpList(AST_NODE* opList)
{
    AST_NODE* currentNode = opList;
    AST_NODE* previousNode;

    while(currentNode != NULL)
    {
        previousNode = currentNode;
        currentNode = currentNode->next;
        free(previousNode);
    }

}

//Lambda
SYMBOL_TABLE_NODE* addSymbolToArgList(char* symbolToAdd, SYMBOL_TABLE_NODE* arg_list)
{
    SYMBOL_TABLE_NODE* toAdd = malloc(sizeof(SYMBOL_TABLE_NODE));

    toAdd->ident = malloc((strlen(symbolToAdd) * sizeof(char))+ 1);
    //toAdd->type = VARIABLE_TYPE;
    strcpy(toAdd->ident, symbolToAdd);
    toAdd->type = ARG_TYPE;
    toAdd->next = NULL;
    toAdd->stack = NULL;

    if(arg_list == NULL)
    {
        return toAdd;
    }

    toAdd->next = arg_list;

    return toAdd;
}

SYMBOL_TABLE_NODE* createLambda(DATA_TYPE data_type, char* symbol, SYMBOL_TABLE_NODE* arg_list, AST_NODE* s_expr)
{
    SYMBOL_TABLE_NODE* lambda = malloc(sizeof(SYMBOL_TABLE_NODE));
    lambda->ident = malloc((strlen(symbol) * sizeof(char))+ 1);
    strcpy(lambda->ident, symbol);

    lambda->val_type = data_type;
    lambda->type = LAMBDA_TYPE;

    s_expr->symbolTable = arg_list;

    lambda->val = s_expr;

    return lambda;
}




//Variables
SYMBOL_TABLE_NODE* createSymbolTable(char* symbol, AST_NODE* s_expr)
{
    SYMBOL_TABLE_NODE *p;
    size_t nodeSize = sizeof(SYMBOL_TABLE_NODE);
    if((p = malloc(nodeSize)) == NULL)
    {
        yyerror("Out of memory");
    }

    p->ident = symbol;
    if(s_expr != NULL)
    {
        p->val = s_expr;
    }
    p->next = NULL;

    return p;
}

SYMBOL_TABLE_NODE* addSymbolToList(SYMBOL_TABLE_NODE* let_list, SYMBOL_TABLE_NODE* let_elem)
{
    //printf("Running\n");
    if(let_elem == NULL|| let_elem->val == NULL)
    {
        return let_list;
    }

    SYMBOL_TABLE_NODE* foundNode = findSymbol(let_list, let_elem);

    if(foundNode == NULL)
    {
        let_elem ->next = let_list;

        ////Double check if needed 8888888888888888888888888888888888888888888888888888888888888888888888888888888888888
        let_elem-> val->parent = let_list->val->parent;
        ////

        return let_elem;
    }
    else
    {
        foundNode ->val = let_elem->val;
        //Double check if needed

        free(let_elem);

        return let_list;
    }

    //Check for duplicate
//    bool isDuplicate = false;
//    SYMBOL_TABLE_NODE* currentNode = let_list;
//    while((!isDuplicate)&& (let_list != NULL))
//    {
//        if(strcmp((let_elem->ident), (currentNode->ident)) == 0)
//        {
//            isDuplicate = true;
//        }
//
//        currentNode = currentNode->next;
//    }
//
//    if(isDuplicate)
//    {
//        //yyerror("Error symbol has been declared previously \n");
//    }
//
//    //Add let_elem to beginning of symbol table list
//    let_elem->val->parent = let_list->val->parent;
//    let_elem->next = let_list;
//    return let_elem;
}

AST_NODE* setSymbolTable(SYMBOL_TABLE_NODE *let_section ,AST_NODE* s_expr)
{
    if(s_expr == NULL)
    {
        return NULL;
    }

    SYMBOL_TABLE_NODE* currentNode = let_section;
    s_expr->symbolTable = let_section;

    while(currentNode != NULL)
    {
        currentNode ->val->parent = s_expr;

        currentNode = currentNode->next;
    }
    fprintf(stderr, s_expr != NULL ? "Not NULLLLLLLL\n" : "NOT NULL\n");
    return s_expr;

}


SYMBOL_TABLE_NODE* createSymbol(DATA_TYPE val_type, char* symbolName, AST_NODE* s_expr)
{
	//Creates let_elem
	SYMBOL_TABLE_NODE* let_elem;
	if(s_expr == NULL)
    {
        return NULL;
    }
	if((let_elem = malloc(sizeof(SYMBOL_TABLE_NODE))) == 0)
	{
		yyerror("Out of memory");
	}

	let_elem->next = NULL;
	//Set symbol type to variable type
	let_elem->type = VARIABLE_TYPE;

	//Copy name
	let_elem->ident = malloc(sizeof(char) * 65);
	strcpy(let_elem->ident, symbolName);

	//Set data type
	let_elem->val_type = val_type;

	//Evaluate s_expr

	//For Read
	if(s_expr->type == FUNC_TYPE && (strcmp(s_expr->data.function.name, "read") == 0))
    {
	    printf("%s ", symbolName);
    }
	RETURN_VALUE* tempVal = eval(s_expr);
	AST_NODE* newVal = malloc(sizeof(AST_NODE));

	newVal->type = NUM_TYPE;
	newVal->data.number.data_type = tempVal->type;

	switch(tempVal->type)
    {
        case INTEGER_TYPE:
            newVal->data.number.value.intVal = round(tempVal->value);
            break;
        default:
            newVal->data.number.value.realVal = tempVal->value;
            break;
    }

	let_elem->val = newVal;
	let_elem->next = NULL;

	free(tempVal);

	return let_elem;


}

AST_NODE* symbol(char* symbol)
{
    AST_NODE* node;

    if ((node = (AST_NODE*) malloc(sizeof(SYMBOL_TABLE_NODE))) == NULL)
    {
        yyerror("out of memory");
    }



    node->type = SYM_TYPE;
    node->data.symbol.name = malloc((sizeof(char) * 65));
    strcpy(node->data.symbol.name, symbol);
    node->parent = NULL;
    node->symbolTable = NULL;

    return node;

}

//Looks for symbol
SYMBOL_TABLE_NODE* findSymbol(SYMBOL_TABLE_NODE* symTable, SYMBOL_TABLE_NODE* sym)
{
    //WHEN SYMBOL IS NULL
    if(sym == NULL)
    {
        return NULL;
    }

    //Search for symbol in ONLY current symbol table
    SYMBOL_TABLE_NODE* currentNode = symTable;

    while(currentNode != NULL)
    {
        //printf("Find Symbol 2!\n");
        if(strcmp(currentNode->ident, sym->ident) == 0 && currentNode->type == VARIABLE_TYPE)
        {
            return currentNode;
        }

        currentNode = currentNode->next;
    }
    //


    //CASE WHERE SYM NOT FOUND IN SYMBOLTABLE
    return NULL;
}



//resolveSymbol
SYMBOL_TABLE_NODE* searchSymbol(AST_NODE *p)
{
    //Check if symbol is in symboltable
    AST_NODE* currentScope = p;
    SYMBOL_TABLE_NODE* currentNode;

    while(currentScope != NULL)
    {
        //printf("searchSym!\n");
        currentNode = currentScope->symbolTable;

        while(currentNode != NULL)
        {
            if((currentNode->ident != NULL) && strcmp(currentNode->ident, p->data.symbol.name) == 0)
            {
                fprintf(stderr, "\tFound %s value\n", currentNode->ident);
                return currentNode;
            }

            currentNode = currentNode->next;
        }

        currentScope = currentScope->parent;
    }

    printf("Search SYM Finish\n");

    return NULL;

}

RETURN_VALUE* evalSymbol(AST_NODE* p)
{
    SYMBOL_TABLE_NODE* symbol = searchSymbol(p);

    RETURN_VALUE* returnValue = NULL;

    if(symbol == NULL)
    {
        yyerror("UNDEFINED SYMBOL REFERENCED");
    }

    else
    {
        switch (symbol->type)
        {
            case VARIABLE_TYPE:
                returnValue = malloc(sizeof(RETURN_VALUE));
                returnValue->type = symbol->val_type;
                RETURN_VALUE* tempVal = eval(symbol->val);

                //Display precision loss warning
                if(returnValue->type != tempVal->type)
                {
                    printf("WARNING: precision loss\n");
                }

                returnValue->value = tempVal->value;
                if(returnValue->type == INTEGER_TYPE)
                {
                    returnValue->value = round(returnValue->value);
                }
                free(tempVal);
                break;

            case ARG_TYPE:
                //Pop from stack
                returnValue = popArgFromStack(symbol);
                break;

            default:
                fprintf(stderr, "\t\t DEBUG: ERROR IN SYMBOL TYPE IN EVAL SYMBOL\n");
        }
        return returnValue;
    }

    return returnValue;
}

bool checkIfSameDataType(RETURN_VALUE* op1, RETURN_VALUE* op2)
{
    if(op1->type == op2->type)
    {
        return true;
    }
    else if(((op1->type == NO_TYPE)||(op1->type == REAL_TYPE)) && ((op2->type == NO_TYPE)||(op2->type == REAL_TYPE)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void printValue(AST_NODE* opList)
{

    fprintf(stderr,"DEBUG:   PRINTING VALUES\n");
    RETURN_VALUE * toPrint;
    AST_NODE* currentNode = opList;

    while(currentNode != NULL)
    {
        toPrint = eval(currentNode);
        fprintf(stderr,"DEBUG:   PRINTING VALUE\n");


        switch(toPrint->type)
        {
            case NO_TYPE:
            case REAL_TYPE:
                printf("%lf ", (toPrint->value));
                break;
            case INTEGER_TYPE:
                printf("%d ", (int)(round(toPrint->value)));
                break;
            default:
                yyerror("Print Instruction For Unknown Data Type");
                break;
        }

        currentNode = currentNode->next;

    }

    //Print new line
    printf("\n");



}



bool tooManyOps(OPER_TYPE operType, AST_NODE* opList)
{
    int count = 0;
    bool tooMany = false;
    AST_NODE* currentNode = opList;
    while(currentNode != NULL)
    {
        count++;
        currentNode = currentNode->next;
    }

    switch(operType)
    {
        case HYPOT_OPER:
        case SUB_OPER:
        case DIV_OPER:
        case MAX_OPER:
        case MIN_OPER:
        case POW_OPER:
        case REMAINDER_OPER:
        case SMALLER_OPER:
        case LARGER_OPER:
        case EQUAL_OPER:
            if(count > 2)
            {
                fprintf(stderr, "******FUNCTION WITH TOO MANY\n");
                tooMany = true;
            }
            break;
        case ABS_OPER:
        case NEG_OPER:
        case LOG_OPER:
        case EXP2_OPER:
        case SQRT_OPER:
        case CBRT_OPER:
            if(count > 1)
            {
                fprintf(stderr, "******FUNCTION WITH TOO MANY\n");
                tooMany = true;
            }
            break;
        case READ_OPER:
        case RAND_OPER:
            if(count > 0)
            {
                yyerror("FUNCTION CONTAINS TOO MANY OPERANDS");
                tooMany = true;
            }
            break;

        default:
            yyerror("ERROR COULDN'T ID FUNCTION TYPE");
            tooMany = true;


    }

    return tooMany;

}


bool tooFewOps(OPER_TYPE operType, AST_NODE* opList)
{
    int count = 0;
    bool tooFew = false;
    AST_NODE* currentNode = opList;
    while(currentNode != NULL)
    {
        count++;
        currentNode = currentNode->next;
    }



    switch(operType)
    {
        case PRINT_OPER:
            if(count < 1)
            {
                fprintf(stderr, "******FUNCTION WITH TOO FEW\n");
                tooFew = true;
            }
            break;
        case ADD_OPER:
        case MULT_OPER:
        case HYPOT_OPER:
        case SUB_OPER:
        case DIV_OPER:
        case MAX_OPER:
        case MIN_OPER:
        case POW_OPER:
        case REMAINDER_OPER:
        case SMALLER_OPER:
        case LARGER_OPER:
        case EQUAL_OPER:
            if(count < 2)
            {
                fprintf(stderr, "******FUNCTION WITH TOO FEW\n");
                tooFew = true;
            }
            break;
        case ABS_OPER:
        case NEG_OPER:
        case LOG_OPER:
        case EXP2_OPER:
        case SQRT_OPER:
        case CBRT_OPER:
            if(count < 1)
            {
                fprintf(stderr, "******FUNCTION WITH TOO FEW\n");
                tooFew = true;
            }
            break;
        case READ_OPER:
        case RAND_OPER:
            //Don't do anything
            break;

        default:
            yyerror("ERROR COULDN'T ID FUNCTION TYPE *** TOO FEW");
            tooFew = true;


    }

    return tooFew;

}

//Add operations together
RETURN_VALUE* addOps(AST_NODE* opList)
{
    AST_NODE* currentNode = opList->next;
    RETURN_VALUE* sum = eval(opList);


    while(currentNode != NULL)
    {
        sum->value += eval(currentNode)->value;
        currentNode = currentNode->next;
    }

    return sum;
}

//Multiplies operations together
RETURN_VALUE* multOps(AST_NODE* opList)
{
    AST_NODE* currentNode = opList->next;
    RETURN_VALUE* product = eval(opList);
    RETURN_VALUE* op2;


    while(currentNode != NULL)
    {
        op2 = eval(currentNode);

        if(!checkIfSameDataType(product, op2))
        {
            product ->type = REAL_TYPE;
        }

        product->value *= op2->value;

        currentNode = currentNode->next;
    }

    return product;
}


AST_NODE* setConditional(AST_NODE* condExpr, AST_NODE* trueExpr, AST_NODE* falseExpr)
{
    AST_NODE* p = malloc(sizeof(AST_NODE));

    p->data.cond = malloc(sizeof(COND_AST_NODE));
    p->type = COND_TYPE;

    p->data.cond->condExpr = condExpr;
    condExpr->parent = p;

    p->data.cond->trueExpr = trueExpr;
    trueExpr->parent = p;

    p->data.cond->falseExpr = falseExpr;
    falseExpr->parent = p;

    return p;
}


//
// evaluate an abstract syntax tree
//
// p points to the root
//
RETURN_VALUE* eval(AST_NODE *p)
{
    //int func;
   // printf("DEBUG:       EVAL\n");
    RETURN_VALUE* returnValue;


    if (!p)
    {
        returnValue = malloc(sizeof(RETURN_VALUE));
        returnValue->type = NO_TYPE;
        returnValue->value = 0.0;
        return returnValue;
    }

// TODO: implement
    RETURN_VALUE* op1;
    RETURN_VALUE* op2;
    switch(p->type)
    {
        case NUM_TYPE:
            returnValue = malloc(sizeof(RETURN_VALUE));
            returnValue->type = p->data.number.data_type;
            //printf("Data Type: %d ==>", (returnValue->type));
            switch (returnValue->type)
            {
                case NO_TYPE:
                case REAL_TYPE:
                    returnValue->value = p->data.number.value.realVal;
                    //printf("%lf\n",p->data.number.value.realVal);
                    break;
                case INTEGER_TYPE:
                    returnValue->value = p->data.number.value.intVal;
                    //printf("%d\n", (int)p->data.number.value.intVal);
                    break;
                default:
                    yyerror("No data type in eval NUM_TYPE");
                    break;
            }
            return returnValue;
        case FUNC_TYPE:
            //func = resolveFunc(p->data.function.name);
            switch(resolveFunc(p->data.function.name))
            {

                case NEG_OPER:
                    if(tooFewOps(NEG_OPER, p->data.function.opList))
                    {
                        //If too few ops, return 0
                        returnValue = malloc(sizeof(RETURN_VALUE));
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }

                    //Check if too many ops
                    tooManyOps(NEG_OPER,p->data.function.opList);


                    op1 = eval(p->data.function.opList);
                    op1->value *= (-1);
                    return op1;

                case ABS_OPER:
                    if(tooFewOps(ABS_OPER, p->data.function.opList))
                    {
                        //If too few ops, return 0
                        returnValue = malloc(sizeof(RETURN_VALUE));
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }
                    tooManyOps(ABS_OPER, p->data.function.opList);


                    op1 = eval(p->data.function.opList);
                    //op1 = eval(p->data.function.op1);
                    op1->value = fabs(op1->value);
                    return op1;
                    //return fabs(eval(p->data.function.op1));
                case EXP_OPER:
                    if(tooFewOps(EXP_OPER, p->data.function.opList))
                    {
                        //If too few ops, return 0
                        returnValue = malloc(sizeof(RETURN_VALUE));
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }
                    tooManyOps(EXP_OPER, p->data.function.opList);


                    op1 = eval(p->data.function.opList);
                    //op1 = eval(p->data.function.op1);
                    op1->value = exp(op1->value);
                    if(op1->type == INTEGER_TYPE)
                    {
                        op1->value = round(op1->value);
                    }
                    return op1;
                    //return exp(eval(p->data.function.op1));
                case SQRT_OPER:
                    if(tooFewOps(SQRT_OPER, p->data.function.opList))
                    {
                        //If too few ops, return 0
                        returnValue = malloc(sizeof(RETURN_VALUE));
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }
                    //Check for too many operations
                    tooManyOps(SQRT_OPER, p->data.function.opList);

                    op1 = eval(p->data.function.opList);
                    //op1 = eval(p->data.function.op1);
                    op1->value = sqrt(op1->value);

                    if(op1->type == INTEGER_TYPE)
                    {
                        op1->value = round(op1->value);
                    }
                    return op1;
                    //return sqrt(eval(p->data.function.op1));
                case ADD_OPER:
                    if(tooFewOps(ADD_OPER, p->data.function.opList))
                    {
                        //If too few ops, return 0
                        returnValue = malloc(sizeof(RETURN_VALUE));
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }

                    //Add operations together
                    op1 = addOps(p->data.function.opList);

//                    op1 = eval(p->data.function.op1);
//                    op2 = eval(p->data.function.op2);
//
//                    //Check if they are two different data types
//                    if(!(checkIfSameDataType(op1, op2)))
//                    {
//                        printf("WARNING: Precision loss!!!!\n");
//                    }
//
//                    op1->value += op2->value;
//                    free(op2);
//
//                    if(op1->value == INTEGER_TYPE)
//                    {
//                        op1->value = round(op1->value);
//                    }

                   fprintf(stderr,"DEBUG ADD:        %lf\n", op1->value);
                    return op1;
                    //return eval(p->data.function.op1) + eval(p->data.function.op2);
                case SUB_OPER:
                    if(tooFewOps(SUB_OPER, p->data.function.opList))
                    {
                        //If too few ops, return 0
                        returnValue = malloc(sizeof(RETURN_VALUE));
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }

                    tooManyOps(SUB_OPER, p->data.function.opList);

                    op1 = eval(p->data.function.opList);
                    op2 = eval(p->data.function.opList->next);

                    //Check if they are two different data types
                    if(!(checkIfSameDataType(op1, op2)))
                    {
                        printf("WARNING: Precision loss!!!!\n");
                    }

                    op1->value -= op2->value;
                    free(op2);

                    if(op1->value == INTEGER_TYPE)
                    {
                        op1->value = round(op1->value);
                    }

                    return op1;
                    //return eval(p->data.function.op1) - eval(p->data.function.op2);
                case MULT_OPER:
//                    op1 = eval(p->data.function.opList);
//                    op2 = eval(p->data.function.opList->next);
//
//                    //Check if they are two different data types
//                    if(!(checkIfSameDataType(op1, op2)))
//                    {
//                        printf("WARNING: Precision loss!!!!\n");
//                        op1->type = REAL_TYPE;
//                    }
//
//                    op1->value *= op2->value;
//                    free(op2);

                    if(tooFewOps(MULT_OPER, p->data.function.opList))
                    {
                        //If too few ops, return 0
                        returnValue = malloc(sizeof(RETURN_VALUE));
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }




                    op1 = multOps(p->data.function.opList);
                    if(op1->value == INTEGER_TYPE)
                    {
                        op1->value = round(op1->value);
                    }

                    return op1;
                    //return eval(p->data.function.op1) * eval(p->data.function.op2);
                case DIV_OPER:
                    if(tooFewOps(DIV_OPER, p->data.function.opList))
                    {
                        //If too few ops, return 0
                        returnValue = malloc(sizeof(RETURN_VALUE));
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }


                    tooManyOps(DIV_OPER, p->data.function.opList);


                    op1 = eval(p->data.function.opList);
                    op2 = eval(p->data.function.opList->next);
                    if(op2->value == 0.0 || op1->value == NAN || op2->value == NAN)
                    {
                        op1->value = NAN;
                        free(op2);

                        return op1;
                    }

                    //Check if they are two different data types
                    if(!(checkIfSameDataType(op1, op2)))
                    {
                        printf("WARNING: Precision loss!!!!\n");
                        op1->type = REAL_TYPE;
                    }

                    op1->value /= op2->value;
                    free(op2);

                    if(op1->value == INTEGER_TYPE)
                    {
                        op1->value = round(op1->value);
                    }

                    return op1;

//                    op1 = eval(p->data.function.op1);
//                    op2 = eval(p->data.function.op2);
//                    if(op2 == 0 || op1 == NAN || op2 == NAN)
//                    {
//                        //return NAN;
//                    }
//                    return op1/op2;
                case REMAINDER_OPER:
                    if(tooFewOps(REMAINDER_OPER, p->data.function.opList))
                    {
                        //If too few ops, return 0
                        returnValue = malloc(sizeof(RETURN_VALUE));
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }


                    tooManyOps(REMAINDER_OPER, p->data.function.opList);


                    op1 = eval(p->data.function.opList);
                    op2 = eval(p->data.function.opList->next);
                    if(op2->value == 0.0 || op1->value == NAN || op2->value == NAN)
                    {
                        op1->value = NAN;
                        free(op2);

                        return op1;
                    }

                    //Check if they are two different data types
                    if(!(checkIfSameDataType(op1, op2)))
                    {
                        printf("WARNING: Precision loss!!!!\n");
                        op1->type = REAL_TYPE;
                    }

                    op1->value = remainder(op1->value, op2->value);
                    free(op2);

                    if(op1->value == INTEGER_TYPE)
                    {
                        op1->value = round(op1->value);
                    }

                    return op1;
//                    return remainder(eval(p->data.function.op1), eval(p->data.function.op1));

                case LOG_OPER:
                    if(tooFewOps(LOG_OPER, p->data.function.opList))
                    {
                        //If too few ops, return 0
                        returnValue = malloc(sizeof(RETURN_VALUE));
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }

                    tooManyOps(LOG_OPER, p->data.function.opList);

                    op1 = eval(p->data.function.opList);
                    //op1 = eval(p->data.function.op1);
                    op1->value = log(op1->value);
                    if(op1->type == INTEGER_TYPE)
                    {
                        op1->value = round(op1->value);
                    }
                    return op1;
//                    return log(eval(p->data.function.op1));
                case POW_OPER:
                    if(tooFewOps(POW_OPER, p->data.function.opList))
                    {
                        //If too few ops, return 0
                        returnValue = malloc(sizeof(RETURN_VALUE));
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }


                    tooManyOps(POW_OPER, p->data.function.opList);

                    op1 = eval(p->data.function.opList);
                    op2 = eval(p->data.function.opList->next);

                    //Check if they are two different data types
                    if(!(checkIfSameDataType(op1, op2)))
                    {
                        printf("WARNING: Precision loss!!!!\n");
                    }

                    op1->value = pow(op1->value, op2->value);
                    free(op2);

                    if(op1->value == INTEGER_TYPE)
                    {
                        op1->value = round(op1->value);
                    }

                    return op1;
                    //return pow(eval(p->data.function.op1), eval(p->data.function.op2));
                case MAX_OPER:
                    if(tooFewOps(MAX_OPER, p->data.function.opList))
                    {
                        //If too few ops, return 0
                        returnValue = malloc(sizeof(RETURN_VALUE));
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }

                    tooManyOps(MAX_OPER, p->data.function.opList);


                    op1 = eval(p->data.function.opList);
                    op2 = eval(p->data.function.opList->next);
                    if(fmax(op1->value, op2->value) == op1->value)
                    {
                        return op1;
                    } else
                    {
                        return op2;
                    }
                    //return fmax(eval(p->data.function.op1), eval(p->data.function.op2));
                case MIN_OPER:
                    if(tooFewOps(MIN_OPER, p->data.function.opList))
                    {
                        //If too few ops, return 0
                        returnValue = malloc(sizeof(RETURN_VALUE));
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }

                    tooManyOps(MIN_OPER, p->data.function.opList);


                    op1 = eval(p->data.function.opList);
                    op2 = eval(p->data.function.opList->next);
                    if(fmax(op1->value, op2->value) == op1->value)
                    {
                        return op1;
                    } else
                    {
                        return op2;
                    }
                    //return fmin(eval(p->data.function.op1), eval(p->data.function.op2));
                case EXP2_OPER:
                    if(tooFewOps(EXP2_OPER, p->data.function.opList))
                    {
                        //If too few ops, return 0
                        returnValue = malloc(sizeof(RETURN_VALUE));
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }

                    tooManyOps(EXP2_OPER, p->data.function.opList);

                    op1 = eval(p->data.function.opList);
                    //op1 = eval(p->data.function.op1);
                    op1->value = exp2(op1->value);
                    if(op1->type == INTEGER_TYPE)
                    {
                        op1->type = round(op1->type);
                    }
                    return op1;
                    //return exp2(eval(p->data.function.op1));
                case CBRT_OPER:
                    if(tooFewOps(CBRT_OPER, p->data.function.opList))
                    {
                        //If too few ops, return 0
                        returnValue = malloc(sizeof(RETURN_VALUE));
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }


                    tooManyOps(CBRT_OPER, p->data.function.opList);

                    op1 = eval(p->data.function.opList);
                    //op1 = eval(p->data.function.op1);
                    op1->value = cbrt(op1->value);
                    if(op1->type == INTEGER_TYPE)
                    {
                        op1->type = round(op1->type);
                    }
                    return op1;
                    //return cbrt(eval(p->data.function.op1));
                case HYPOT_OPER:
                    if(tooFewOps(HYPOT_OPER, p->data.function.opList))
                    {
                        //If too few ops, return 0
                        returnValue = malloc(sizeof(RETURN_VALUE));
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }

                    tooManyOps(HYPOT_OPER, p->data.function.opList);

                    op1 = eval(p->data.function.opList);
                    op2 = eval(p->data.function.opList->next);

                    op1->value = hypot(op1->value, op2->value);
                    if(checkIfSameDataType(op1, op2) && op1->type == INTEGER_TYPE)
                    {
                        op1->value = round(op1->value);
                    }
                    free(op2);
                    return op1;
                    //return hypot(eval(p->data.function.op1), eval(p->data.function.op2));

                case PRINT_OPER:

                    if(tooFewOps(PRINT_OPER, p->data.function.opList))
                    {
                        //If too few ops, return 0
                        returnValue = malloc(sizeof(RETURN_VALUE));
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }

                   //printf("Print\n");
                    printValue(p->data.function.opList);

                    returnValue = malloc(sizeof(RETURN_VALUE));
                    returnValue->type = NO_TYPE;
                    returnValue->value = 0.0;
                    return returnValue;


                case READ_OPER:
                    //Check if there are any operands
                    tooManyOps(READ_OPER, p->data.function.opList);

                    //Receive input from user
                    returnValue = malloc(sizeof(RETURN_VALUE));
                    returnValue->type = NO_TYPE;

                    printf("<read> := ");
                    scanf("%lf", &(returnValue->value));

                    //Clear newline character from buffer
                    getchar();

                    fprintf(stderr,"\t\tDEBUG: READ NUMBER ==> %lf\n", returnValue->value);

                    return returnValue;

                case RAND_OPER:
                    //Check if there are any operands
                    tooManyOps(RAND_OPER, p->data.function.opList);

                    returnValue = malloc(sizeof(RETURN_VALUE));
                    returnValue->type = NO_TYPE;

                    //Generate random double
                    returnValue->value = ((double)rand())/rand();

                    fprintf(stderr, "\t\tDEBUG: random value ==> %lf\n", returnValue->value);

                    return returnValue;

                case SMALLER_OPER:
                    //TODO: FINISH
                    returnValue = malloc(sizeof(RETURN_VALUE));
                    //If less than 2 operands
                    if(tooFewOps(SMALLER_OPER, p->data.function.opList))
                    {
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }

                    //Check if there are more than 2 operands
                    tooManyOps(SMALLER_OPER, p->data.function.opList);

                    //If op1 less than op2
                    //0 is false
                    //1 is true
                    op1 = eval(p->data.function.opList);
                    op2 = eval(p->data.function.opList->next);

                    if((fabs((op1->value) - (op2->value)) < 0.00001) || (op1->value > op2->value))
                    {
                        //If equal or op1 > op2
                        returnValue->value = 0.0;
                    }
                    else
                    {
                        //op1 < op2
                        returnValue->value = 1.0;
                    }

                    return returnValue;


                case LARGER_OPER:
                    //TODO: FINISH
                    returnValue = malloc(sizeof(RETURN_VALUE));
                    //If less than 2 operands
                    if(tooFewOps(SMALLER_OPER, p->data.function.opList))
                    {
                        returnValue->type = NO_TYPE;
                        returnValue->value = 0.0;

                        return returnValue;
                    }

                    //Check if there are more than 2 operands
                    tooManyOps(SMALLER_OPER, p->data.function.opList);

                    //If op1 greater than op2
                    //0 is false
                    //1 is true
                    op1 = eval(p->data.function.opList);
                    op2 = eval(p->data.function.opList->next);

                    if((fabs((op1->value) - (op2->value)) < 0.00001) || (op1->value < op2->value))
                    {
                        //If equal or op1 < op2
                        returnValue->value = 0.0;
                    }
                    else
                    {
                        //op1 > op2
                        returnValue->value = 1.0;
                    }

                    return returnValue;

                case EQUAL_OPER:
                    returnValue = malloc(sizeof(RETURN_VALUE));
                    returnValue->type = NO_TYPE;
                    //If less than 2 operands
                    if(tooFewOps(SMALLER_OPER, p->data.function.opList))
                    {

                        returnValue->value = 0.0;

                        return returnValue;
                    }

                    //Check if there are more than 2 operands
                    tooManyOps(SMALLER_OPER, p->data.function.opList);

                    op1 = eval(p->data.function.opList);
                    op2 = eval(p->data.function.opList->next);

                    //If they are equal
                    //0 is false
                    //1 is true
                    if(fabs((op1->value) - (op2->value)) < 0.00001)
                    {
                        //If equal
                        returnValue->value = 1.0;
                    } else
                    {
                        //If not equal
                        returnValue->value = 0.0;
                    }


                    return returnValue;


                case CUSTOM_FUNC:
                    return evalCustomFunc(p);
                   //return NULL;

                default:
                    printf("WOOPSIE\n");
                    break;

            }
            break;
        case SYM_TYPE:
            //switch(p->parent->symbolTable->type)
            //{
                //case VARIABLE_TYPE:
                //case ARG_TYPE:
                return evalSymbol(p);
                //case LAMBDA_TYPE:
                    //return NULL;

                //case ARG_TYPE:
                   // return  NULL;

                //default:
                    //yyerror("THERE WAS NO VALID SYMBOL_TYPE\n");
                    //break;
            //}






        case COND_TYPE:

            //If true
            if(eval(p->data.cond->condExpr)->value == 1.0)
            {
                returnValue = eval(p->data.cond->trueExpr);
            } else
            {
                returnValue = eval(p->data.cond->falseExpr);
            }

            //Free cond
            free(p->data.cond);

            return returnValue;

        default:
            printf("SOME REASON TYPE WAS NOT ANY ENUM\n");
            break;

    }
    returnValue = malloc(sizeof(RETURN_VALUE));
    returnValue->type = NO_TYPE;
    returnValue->value = 0.0;
    return returnValue;
}

RETURN_VALUE* popArgFromStack(SYMBOL_TABLE_NODE* symbolTableNode)
{
    RETURN_VALUE* returnValue = eval(symbolTableNode->stack->val);

    freeNode(symbolTableNode->stack->val);

    STACK_NODE* tempNode = symbolTableNode->stack->next;
    free(symbolTableNode->stack);
    symbolTableNode->stack = tempNode;

    return  returnValue;
}

bool pushValueToStackLambda(AST_NODE* p, SYMBOL_TABLE_NODE* lambda)
{
    bool added = false;
    RETURN_VALUE* toPushToStack;

    AST_NODE* currentOp = p->data.function.opList;
    SYMBOL_TABLE_NODE* currentArg = lambda->val->symbolTable;

    while(currentOp != NULL && currentArg != NULL)
    {
        if((currentArg->type) == ARG_TYPE)
        {
            toPushToStack = eval(currentOp);
            pushToStack(toPushToStack, currentArg);

            currentOp = currentOp->next;

            added = true;

        }
//        else
//        {
//            //If currentArg isn't a arg_type
//            currentArg = currentArg->next;
//        }
        currentArg = currentArg->next;
    }


    return added;
}




bool pushToStack(RETURN_VALUE* toPush, SYMBOL_TABLE_NODE* currentArg)
{
    bool added = true;

    if(toPush == NULL || currentArg == NULL)
    {
        added = false;
        return added;
    }

    //Create AST_NODE to add to stack
    AST_NODE* newHead = malloc(sizeof(AST_NODE));

    newHead->type = NUM_TYPE;

    newHead->data.number.data_type = toPush->type;
    if(toPush->type == INTEGER_TYPE)
    {
        newHead->data.number.value.intVal = (int)round(toPush->value);
    } else
    {
        newHead->data.number.value.realVal = toPush->value;
    }

    //Push newHead to stack

    STACK_NODE* newHeadStack = malloc(sizeof(STACK_NODE));

    newHeadStack->val = newHead;

    newHeadStack->next = currentArg->stack;

    currentArg->stack = newHeadStack;

    return added;
}

RETURN_VALUE* evalCustomFunc(AST_NODE* p)
{
    RETURN_VALUE* returnValue;
    SYMBOL_TABLE_NODE* lambda = findLambda(p);

    if(lambda == NULL)
    {
        yyerror("FUNCTION ISN'T FOUND... \n");
        return NULL;
    }

    pushValueToStackLambda(p, lambda);

    returnValue = eval(lambda->val);






    return returnValue;
}

SYMBOL_TABLE_NODE* findLambda(AST_NODE* p)
{
    SYMBOL_TABLE_NODE* currentNode;
    AST_NODE* currentScope = p;

    while (currentScope != NULL)
    {
        currentNode = currentScope->symbolTable;
        while(currentNode != NULL)
        {
            if(strcmp(p->data.function.name, currentNode->ident) == 0 && currentNode->type == LAMBDA_TYPE)
            {
                return currentNode;
            }

            currentNode = currentNode->next;
        }

        currentScope = currentScope->parent;
    }

    return NULL;
}

