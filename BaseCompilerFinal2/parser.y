%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



#include "symbolTable.h"
#include "ConsoleFormating.h"
#include "IRcode.h"
//#include "IROptimization.h"
#include "AST.h"


extern int yylex();
extern int yyparse();
extern FILE* yyin;
FILE * IRcode;


void yyerror(const char* s);
char currentScope[50]; // global or the name of the function
char productionRule[100] = "";
%}

%union 
{
	int number;
	char character;
	char* string;
	struct ASTNode* ast;
}

%token <string> TYPE
%token <string> IDENTIFIER
%token <char> SEMICOLON
%token <char> LCBRA
%token <char> RCBRA
%token <char> EQ
%token <string> OPERATOR
%token <number> NUMBER
%token WRITE


%printer { fprintf(yyoutput, "%s", $$); } OPERATOR;
%printer { fprintf(yyoutput, "%s", $$); } IDENTIFIER;
%printer { fprintf(yyoutput, "%d", $$); } NUMBER;


%type <ast> Program Block BlockList VarDecl Stmt StmtList Expr NonAssignExpr

/*
	Production Rules:

	Block: LCBRA BlockList RCBRA
	BlockList: e | BlockList StmtList | BlockList VarDecl

	VarDecl: TYPE IDENTIFIER SEMICOLON	

	StmtList: stmt StmtList | stmt
	Stmt: SEMICOLON | Expr SEMICOLON
	Expr: IDENTIFER | IDENTIFIER EQ NonAssignExpr | WRITE IDENTIFIER
	NonAssignExpr: IDENTIFER | IDENTIFIER OPERATOR NonAssignExpr | NUMBER OPERATOR NonAssignExpr | NUMBER

*/


%start Program

%%

Program: Block
{ 
	$$ = createNode(NULL, $1, NODE_PROGRAM);
	printf(ANSI_COLOR_BRIGHT_YELLOW"\n--- Current Symbol Table ---\n\n"ANSI_COLOR_RESET);
	PerformLivenessAnalysis();
	showSymTable();
	printf(ANSI_COLOR_BRIGHT_YELLOW"\n--- Abstract Syntax Tree ---\n\n"ANSI_COLOR_RESET);
	printAST($$, 0, 0);
	printf(ANSI_COLOR_BRIGHT_YELLOW"\n--- IR Code ---\n\n"ANSI_COLOR_RESET);
	WriteIRCodeToFile($$);
	freeAST($$); //free non-vars

};

//-----------------------------------------------------------------------------------------------------------------------------

Block: LCBRA BlockList RCBRA { $$ = $2; };

//-----------------------------------------------------------------------------------------------------------------------------

BlockList: %empty
{
	$$ = NULL;
}
| BlockList StmtList
{
	$$ = createNode($1,$2, NODE_BLOCKLIST);
}
| BlockList VarDecl
{
	$$ = createNode($1,$2, NODE_BLOCKLIST);
}

//-----------------------------------------------------------------------------------------------------------------------------

VarDecl: TYPE IDENTIFIER SEMICOLON	
{ 
	
	// Symbol Table
	symTabAccess();
	int inSymTab = found($2, currentScope);
									
	if (inSymTab == 0)
	{
		printf(ANSI_COLOR_BRIGHT_CYAN "\n RECOGNIZED RULE: declaration for variable: %s\n" ANSI_COLOR_RESET, $2); 
		struct ASTNode* ver_decl = createVariableDeclarationNode($2, $1); 
		// Add Item to the symbol table
		addItem($2, "Var", $1, 0, currentScope, ver_decl);
		$$ = ver_decl;
	}
	else
	{
		printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Var %s is already in the symbol table\n"ANSI_COLOR_RESET, $2);
		exit(1);
	}
	showSymTable();
};

//-----------------------------------------------------------------------------------------------------------------------------

StmtList: Stmt StmtList	
{ 
	if($1 != NULL) 
		$$ = createNode($1, $2, NODE_STMT);  
	else
		$$ = $2;
}
| Stmt 
{ 
	if($1 != NULL)
		$$ = $1; 
	else
		$$ = NULL; 

};

//-----------------------------------------------------------------------------------------------------------------------------

Stmt: SEMICOLON	
{
	printf(ANSI_COLOR_BRIGHT_MAGENTA " 	RECOGNIZED RULE: stmt -> SEMICOLON "ANSI_COLOR_RESET);
}
| Expr SEMICOLON	
{	
	printf(ANSI_COLOR_BRIGHT_MAGENTA " 	RECOGNIZED RULE: stmt -> Expr SEMICOLON \n"ANSI_COLOR_RESET);
	if($1 != NULL)
		$$ = createNode(NULL, $1, NODE_STMT);
	else
	{
		$$ = NULL;
	}
};

//-----------------------------------------------------------------------------------------------------------------------------

Expr: IDENTIFIER {}
| IDENTIFIER EQ NonAssignExpr    
{ 
	push("IDENTIFIER EQ");
	// Get a reference to the variable node
	struct ASTNode* identifier_node = TryGetASTNodeRef($1,currentScope);
	
	printf(ANSI_COLOR_BRIGHT_MAGENTA "\n IDENTIFIER %s pointer address: %p"ANSI_COLOR_RESET, $1,(void*)identifier_node);

	if(identifier_node != NULL)
	{
		// -------------------------------- Manage AST Node Generation --------------------------------
		
		// Check If the variable has already been assigned some value
		int has_assignment_node = DoesVarAlreadyHaveAssignment($1, currentScope);

		// Check if [ = -> Constant ]. If it is a constant then we update the value of the var in the symbol table. 
		// In Practice, it should always simplify to $3->constant in this simple language.
		if($3->node_type == NODE_CONSTANT)
		{

			char str[50];	
			strcpy($3->value, str);

			// Update the symbol table
			int updated_var = TryUpdateVarValue($1, currentScope, str);
			
			// If this is the first assignment of the variable then, create and assignment node.
			if(has_assignment_node == 0)
			{
				// Increment the number of references of identifier [for marking variables dead]
				int update_eval = TryIncrementVarNumRef($1,currentScope);
				// Pass an assignment node
				$$ = createNode(identifier_node, $3, NODE_ASSIGNMENT);
				// update the assignment node reference in the symbol table
				int updated_assin_ref = TryUpdateAssignmentNodeRef($1, currentScope, $$);
			}
			else if(has_assignment_node == 1)
			{
				// If the variable already has an assignment, update the assignment node.
				// Get the assignment node ref
				struct ASTNode* assignment_node = GetAssignmentNodeRef($1, currentScope);
				assignment_node->right = $3;
				assignment_node->left = identifier_node;
				$$ = NULL;
				printf(ANSI_COLOR_BRIGHT_GREEN"\n Variable ReAssignment '%s' was reassigned to '%s' \n" ANSI_COLOR_RESET, $1, $3->value);
			}
		}
		else
		{
			if(has_assignment_node == 0)
			{
				// Increment the number of references of identifier [for marking variables dead]
				int update_eval = TryIncrementVarNumRef($1,currentScope);
				// Pass an assignment node
				$$ = createNode(identifier_node, $3, NODE_ASSIGNMENT);
				// update the assignment node reference in the symbol table
				int updated_assin_ref = TryUpdateAssignmentNodeRef($1, currentScope, $$);
			}
			else if( has_assignment_node == 1)
			{
				// If the variable already has an assignment, update the assignment node.
				//Get the assinmnet node ref
				struct ASTNode* assignment_node = GetAssignmentNodeRef($1, currentScope);
				assignment_node->right = $3;
				assignment_node->left = identifier_node;
				$$ = NULL;
				printf(ANSI_COLOR_BRIGHT_GREEN"\n Variable Reassignment '%s' was reassigned to '%s' \n" ANSI_COLOR_RESET, $1, $3->value);
			}
			
		}

	}
	else
	{
		printf(ANSI_COLOR_BRIGHT_RED"\n SEMANTIC ERROR: identifier '%s' use before declaration \n"ANSI_COLOR_RESET, $1);
		exit(1);
	}

	printStackContents();  

}
| WRITE IDENTIFIER      
{ 
	printf("\n RECOGNIZED RULE: WRITE statement\n");
	push("WRITE IDENTIFIER");
	struct ASTNode* identifier_node = TryGetASTNodeRef($2,currentScope);
	int has_assignment = DoesVarAlreadyHaveAssignment($2, currentScope);


	if (identifier_node != NULL && has_assignment == 1)
    {
		// Increment the number of references of identifier [for marking variables dead]
        int update_eval = TryIncrementVarNumRef($2,currentScope);
		$$ = createNode(NULL, identifier_node, NODE_WRITE);
    }
    else if (identifier_node != NULL && has_assignment == 0)
    {
        printf(ANSI_COLOR_BRIGHT_RED"\n SEMANTIC ERROR: identifier '%s' used before being initialized\n"ANSI_COLOR_RESET, $2);
        exit(1);
    }
    else
    {
        printf(ANSI_COLOR_BRIGHT_RED"\n SEMANTIC ERROR: identifier '%s' not declared or assigned any value \n"ANSI_COLOR_RESET, $2);
        exit(1);
    }
	
	printStackContents(); 
};

//-----------------------------------------------------------------------------------------------------------------------------

NonAssignExpr:IDENTIFIER
{
	push("IDENTIFIER");

	// Get a reference to the variable node
	struct ASTNode* identifier_node = TryGetASTNodeRef($1,currentScope);
	int update_eval = TryIncrementVarNumRef($1,currentScope);
	printf(ANSI_COLOR_BRIGHT_MAGENTA "\n IDENTIFIER %s pointer address: %p"ANSI_COLOR_RESET, $1,(void*)identifier_node);


	if(identifier_node != NULL)
	{
		// -------------------------------- Manage AST Node Generation --------------------------------

		// Try and get the current value from the symbol table
		char* symtable_value = TryGetVarValue($1, currentScope);

		// If we got the value from the symbol table replace the identifier with its value
		if (symtable_value != NULL) 
		{
			// Replace the identifier with its value
			char actualValue[50];
			strcpy(actualValue, symtable_value);
			printf(ANSI_COLOR_GREEN "\n Constant Propogation: Setting '%s' to '%s' " ANSI_COLOR_RESET, $1, actualValue);
			
			// Decrement the number of references of identifier [for marking variables dead]
			int update_eval = TryDecrementVarNumRef($1,currentScope);

			// pass a constant node
			$$ = createConstantNode(actualValue);
		}
		else
		{
			printf(ANSI_COLOR_BRIGHT_RED"\n SEMANTIC ERROR: identifier '%s' used before it was assigned a value \n"ANSI_COLOR_RESET, $1);
		}

		// -------------------------------- Manage MIPS Code Generation -------------------------------- 
	}
	else
    {
        printf(ANSI_COLOR_BRIGHT_RED"\n SEMANTIC ERROR: identifier '%s' not found\n"ANSI_COLOR_RESET, $1);
        exit(1);
    }
		
}
| IDENTIFIER OPERATOR NonAssignExpr 
{
	push("IDENTIFIER OPERATOR");

	// Get a reference to the variable node
	struct ASTNode* identifier_node = TryGetASTNodeRef($1,currentScope);

	// Increment the number of references of identifier [for marking variables dead]
	int update_eval = TryIncrementVarNumRef($1,currentScope);
	printf(ANSI_COLOR_BRIGHT_MAGENTA "\n IDENTIFIER %s pointer address: %p"ANSI_COLOR_RESET, $1,(void*)identifier_node);


	if(identifier_node != NULL)
	{
		// -------------------------------- Constant propagation Optimization --------------------------------
		
		// Try and get the current value of the idetifier from the symbol table
		char* symtable_value = TryGetVarValue($1, currentScope);

		// If we got the value from the symbol table replace the identifier with its value
		if (symtable_value != NULL) 
		{
			// Replace the identifier with its value
			char actualValue[50];
			strcpy(actualValue, symtable_value);
			printf(ANSI_COLOR_GREEN "\n Constant Propogation: Setting '%s' to '%s' " ANSI_COLOR_RESET, $1, actualValue);
			
			// Decrement the number of references of identifier [for marking variables dead]
			int update_eval = TryDecrementVarNumRef($1,currentScope);

			// -------------------------------- Constant folding Optimization --------------------------------

			// Preform constant folding optimization if $3 is a constant, then perform the calculation here, and return just a constant node.
			// Node: For a simple language this ends up repeadedly constant folding to simply to 1 constant every time. 
			if($3->node_type == NODE_CONSTANT)
			{
				char fc_str[50];	

				int folded_constant;

				if(strcmp($2, "+") == 0)
				{
					folded_constant = atoi(actualValue) + atoi($3->value);
				}
				else if(strcmp($2, "-") == 0)
				{
					folded_constant = atoi(actualValue) - atoi($3->value);
				}
				else if(strcmp($2, "*") == 0)
				{
					folded_constant = atoi(actualValue) * atoi($3->value);
				}
				else if(strcmp($2, "/") == 0)
				{
					folded_constant = atoi(actualValue) / atoi($3->value);
				}

				sprintf(fc_str, "%d", folded_constant);

				printf(ANSI_COLOR_GREEN "\n Constant folded: '%d' %s '%d' = '%d' " ANSI_COLOR_RESET, atoi(actualValue), $2, atoi($3->value), folded_constant);

				$$ = createConstantNode(fc_str);
			}
			else
			{
				$$ = createBinaryExpressionNode($2, createConstantNode(actualValue), $3);
			}

			// -------------------------------- -------------------------------- --------------------------------

		}
		else
		{
			printf(ANSI_COLOR_BRIGHT_RED"\n SEMANTIC ERROR: identifier '%s' used before it was assigned a value \n"ANSI_COLOR_RESET, $1);
		}

		// -------------------------------- Manage MIPS Code Generation -------------------------------- 

	}
	else
	{
		printf(ANSI_COLOR_BRIGHT_RED"\n SEMANTIC ERROR: identifier '%s' not found\n"ANSI_COLOR_RESET, $1);
		exit(1);
	}
}
| NUMBER OPERATOR NonAssignExpr 
{
	push("NUMBER OPERATOR");
	
	// -------------------------------- Constant folding Optimization --------------------------------
	
	// Preform constant folding optimization again
	if($3->node_type == NODE_CONSTANT)
	{
		char fc_str[50];	

		int folded_constant;

		if(strcmp($2, "+") == 0)
		{
			folded_constant = $1 + atoi($3->value);
		}
		else if(strcmp($2, "-") == 0)
		{
			folded_constant = $1 - atoi($3->value) ;
		}
		else if(strcmp($2, "*") == 0)
		{
			folded_constant = $1 * atoi($3->value);
		}
		else if(strcmp($2, "/") == 0)
		{
			folded_constant = $1 / atoi($3->value);
		}
		

		sprintf(fc_str, "%d", folded_constant);

		printf(ANSI_COLOR_GREEN "\n Constant folded: '%d' %s '%d' = '%d' " ANSI_COLOR_RESET, $1, $2, atoi($3->value), folded_constant);
		
		$$ = createConstantNode(fc_str);
	}
	else
	{
		char c_str[50];
		sprintf(c_str, "%d", $1);
		$$ = createBinaryExpressionNode($2, createConstantNode(c_str), $3);	
	}

	// -------------------------------- Manage MIPS Code Generation -------------------------------- 
} 
| NUMBER                
{ 
	push("NUMBER");

	// -------------------------------- Manage AST Node Generation --------------------------------
	char str[50];
	sprintf(str, "%d", $1); 

	
	$$ = createConstantNode(str);
	//////
	// -------------------------------- Manage MIPS Code Generation -------------------------------- 
	
};  


%%

int main(int argc, char**argv)
{
/*
	#ifdef YYDEBUG
		yydebug = 1;
	#endif
	////
*/
	printf(ANSI_COLOR_BRIGHT_YELLOW "\n\n#####- COMPILER STARTED -#####\n\n" ANSI_COLOR_RESET);
	
	if (argc > 1){
	  if(!(yyin = fopen(argv[1], "r")))
          {
		perror(argv[1]);
		return(1);
	  }
	}

	yyparse();
	freeSymbolTableVariables(); // free variable nodes from symbol table in the AST
}

void yyerror(const char* s) {
	fprintf(stderr, "Parse error: %s\n", s);
	exit(1);
}
