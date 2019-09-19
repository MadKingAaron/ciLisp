//Aaron Jimenez
// © 2019
//ciLisp
//May 8, 2018
%{
    #include "ciLisp.h"
%}

%union {
    double dval;
    char *sval;
    struct ast_node *astNode;
    struct symbol_table_node *symbolNode;
    struct ast_node *functionNode;
};

%token <sval> FUNC SYMBOL
%token <dval> NUMBER TYPE INTEGER_NUMBER REAL_NUMBER
%token LPAREN RPAREN LET EOL QUIT COND LAMBDA

%type <astNode> s_expr f_expr
%type <symbolNode> let_elem let_section let_list arg_list
%type <functionNode> s_expr_list


%%

program:
    s_expr EOL {
        fprintf(stderr, "yacc: program ::= s_expr EOL\n");
        if ($1) {
        	free(eval($1));
        	//printf("Hello there\n");
            //printf("%lf", eval($1)->value);
            freeNode($1);
        }
    };

s_expr:
    REAL_NUMBER {
    	fprintf(stderr, "yacc: s_expr ::= REAL_NUMBER\n");
         $$ = number(1, $1);
    }
    | INTEGER_NUMBER {
    	fprintf(stderr, "yacc: s_expr ::= INTEGER_NUMBER\n");
        $$ = number(2, $1);
    }
    | f_expr {
        $$ = $1;
    }
    | SYMBOL {
    	fprintf(stderr, "SYMBOL\n");
    	$$ = symbol($1);
     }
    | LPAREN let_section s_expr RPAREN {
    	fprintf(stderr, "LPAREN let_section s_expr RPAREN\n");
    	$$ = setSymbolTable($2, $3);
    }
    | LPAREN COND s_expr s_expr s_expr RPAREN {
    	fprintf(stderr, "yacc: LPAREN COND s_expr s_expr s_expr RPAREN\n");
    	$$ = setConditional($3, $4, $5);
    }

    | QUIT {
        fprintf(stderr, "yacc: s_expr ::= QUIT\n");
        exit(EXIT_SUCCESS);
    }
    | error {
        fprintf(stderr, "yacc: s_expr ::= error\n");
        yyerror("unexpected token");
        $$ = NULL;
    };

f_expr:
    LPAREN FUNC s_expr_list RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC expr RPAREN\n");
        $$ = function($2, $3);
    }
    | LPAREN SYMBOL s_expr_list RPAREN
    {
    	fprintf(stderr, "yacc: s_expr ::= LPAREN SYMBOL expr RPAREN\n");
    	$$ = function($2, $3);
    }

s_expr_list:
	s_expr s_expr_list{
		//Add s_expr to list
		$$ = addFunctionToOpList($1, $2);
	}
	|s_expr{
		$$ = addFunctionToOpList($1, NULL);
	} |
	{
		$$ = NULL;
	}
let_section:
	LPAREN let_list RPAREN {
		fprintf(stderr, "LPAREN let_list RPAREN\n");
		$$ = $2;
	}
	| {
		$$ = NULL;
	};
arg_list:
	SYMBOL arg_list
	{
		$$ = addSymbolToArgList($1, $2);
	}
	| SYMBOL
	{
		$$ = addSymbolToArgList($1, NULL);
	};
let_list:
	let_list let_elem {
	fprintf(stderr, "let_list let_elem\n");
		$$ = addSymbolToList($1, $2);
	}
	| LET let_elem
	{
		$$ = $2;
	};
let_elem:
	LPAREN TYPE SYMBOL s_expr RPAREN {
	fprintf(stderr, "LPAREN type SYMBOL s_expr RPAREN\n");
	$$ = createSymbol($2, $3, $4);
	}
	| LPAREN SYMBOL s_expr RPAREN{
		fprintf(stderr, "LPAREN SYMBOL s_expr RPAREN\n");
                $$ = createSymbol(0, $2, $3);
	}
	| LPAREN TYPE SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN
        {
        	fprintf(stderr, "LPAREN TYPE SYMBOL LPAREN arg_list RPAREN s_expr RPAREN\n");
        	$$ = createLambda($2, $3, $6, $8);
        }
        | LPAREN SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN
        {
        	fprintf(stderr, "LPAREN SYMBOL LPAREN arg_list RPAREN s_expr RPAREN\n");
        	$$ = createLambda(0, $2, $5, $7);
        };





%%

