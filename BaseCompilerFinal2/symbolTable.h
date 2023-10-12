#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

//Symbol table header
#include <string.h>
#include "AST.h" 
#include "ConsoleFormating.h"



struct Entry
{
	int itemID;
	int numRef;
	int is_dead;
	char itemName[50];  //the name of the identifier
	char itemKind[8];  //is it a function or a variable?
	char itemType[8];  // Is it int, char, etc.?
	int arrayLength;
	char scope[50];     // global, or the name of the function
	struct ASTNode* ast_node_ref;
	struct ASTNode* assignment_node_ref;
	char itemValue[50];
};

struct Entry symTabItems[100];
int symTabIndex = 0;

void symTabAccess(void){
	printf("::::> Symbol table accessed.\n");
}

void addItem(char itemName[50], char itemKind[8], char itemType[8], int arrayLength, char scope[50], struct ASTNode* ast_node_ref)
{
		// what about scope? should you add scope to this function?
		symTabItems[symTabIndex].itemID = symTabIndex;
		symTabItems[symTabIndex].numRef = 0;
		symTabItems[symTabIndex].is_dead = 0;
		strcpy(symTabItems[symTabIndex].itemName, itemName);
		strcpy(symTabItems[symTabIndex].itemKind, itemKind);
		strcpy(symTabItems[symTabIndex].itemType, itemType);
		symTabItems[symTabIndex].arrayLength = arrayLength;
		strcpy(symTabItems[symTabIndex].scope, scope);
		symTabItems[symTabIndex].ast_node_ref=ast_node_ref;
		strcpy(symTabItems[symTabIndex].itemValue, "");
		symTabIndex++;
	
}


void showSymTable(){
	printf("itemID  numRef  isDead?  itemName    itemKind    itemType  ArrayLength    itemScope    ASTNode*  CurVal   ASTNode*(assign) \n");
	printf("------------------------------------------------------------------------------------------------------------------------\n");
	for (int i=0; i<symTabIndex; i++){
		printf("%5d %5d %5d %15s  %7s  %7s %6d %15s %p %s %p\n",symTabItems[i].itemID, symTabItems[i].numRef, symTabItems[i].is_dead, symTabItems[i].itemName, symTabItems[i].itemKind, symTabItems[i].itemType, symTabItems[i].arrayLength, symTabItems[i].scope, (void*)symTabItems[i].ast_node_ref, symTabItems[i].itemValue, (void*)symTabItems[i].assignment_node_ref );
	}
	

	printf("------------------------------------------------------------------------------------------------------------------------\n");
}

int found(char itemName[50], char scope[50]){
	// Lookup an identifier in the symbol table
	// what about scope?
	// return TRUE or FALSE
	// Later on, you may want to return additional information
	for(int i=0; i<100; i++){
		int str1 = strcmp(symTabItems[i].itemName, itemName);
		int str2 = strcmp(symTabItems[i].scope,scope);
		if( str1 == 0 && str2 == 0)
		{
			return 1; // found the ID in the table
		}
	}
	return 0;
}

int TryUpdateVarValue(char itemName[50], char scope[50], char value[50])
{
	// Lookup an identifier in the symbol table
	// what about scope?
	// return TRUE or FALSE
	// Later on, you may want to return additional information
	for(int i=0; i<100; i++)
	{
		int str1 = strcmp(symTabItems[i].itemName, itemName);
		int str2 = strcmp(symTabItems[i].scope,scope);
		if( str1 == 0 && str2 == 0)
		{
			strcpy(symTabItems[i].itemValue, value);
			return 1;
		}
	}

	printf(ANSI_COLOR_BRIGHT_YELLOW "\n ERROR: failed to set variable '%s' to '%s' in scope '%s' \n	REASON: -> not found in symbol table \n" ANSI_COLOR_RESET, itemName,  value, scope);
	return 0;
}

int TryUpdateAssignmentNodeRef(char itemName[50], char scope[50], struct ASTNode* assignment_node)
{
	// Lookup an identifier in the symbol table
	// what about scope?
	// return TRUE or FALSE
	// Later on, you may want to return additional information
	for(int i=0; i<100; i++)
	{
		int str1 = strcmp(symTabItems[i].itemName, itemName);
		int str2 = strcmp(symTabItems[i].scope,scope);
		if( str1 == 0 && str2 == 0)
		{
			symTabItems[i].assignment_node_ref = assignment_node;
			return 1;
		}
	}

	printf(ANSI_COLOR_BRIGHT_YELLOW "\n ERROR: failed to set asssignemnt node ref for variable '%s' in scope '%s' \n	REASON: -> not found in symbol table \n" ANSI_COLOR_RESET, itemName,  scope);
	return 0;
}

int DoesVarAlreadyHaveAssignment(char itemName[50], char scope[50])
{
    for(int i=0; i<100; i++)
    {
        int str1 = strcmp(symTabItems[i].itemName, itemName);
        int str2 = strcmp(symTabItems[i].scope, scope);
        if(str1 == 0 && str2 == 0)
        {
            if(symTabItems[i].assignment_node_ref != NULL)
            {
                return 1; // The ast_node_ref is pointing to something
            }
            else
            {
                return 0; // The ast_node_ref is NULL
            }
        }
    }
    return 0; // Entry not found in the symbol table
}

struct ASTNode* GetAssignmentNodeRef(char itemName[50], char scope[50])
{
	for(int i=0; i<100; i++)
    {
        int str1 = strcmp(symTabItems[i].itemName, itemName);
        int str2 = strcmp(symTabItems[i].scope, scope);
        if(str1 == 0 && str2 == 0)
        {
            if(symTabItems[i].assignment_node_ref != NULL)
            {
                return symTabItems[i].assignment_node_ref; // The ast_node_ref is pointing to something
            }
            else
            {
                return NULL; // The ast_node_ref is NULL
            }
        }
    }
    return NULL; // Entry not found in the symbol table
}


int TryIncrementVarNumRef(char itemName[50], char scope[50])
{
	// Lookup an identifier in the symbol table
	// what about scope?
	// return TRUE or FALSE
	// Later on, you may want to return additional information
	for(int i=0; i<100; i++)
	{
		int str1 = strcmp(symTabItems[i].itemName, itemName);
		int str2 = strcmp(symTabItems[i].scope,scope);
		if( str1 == 0 && str2 == 0)
		{
			if(symTabItems[i].numRef + 1 >= 2)
			{
				symTabItems[i].is_dead = 0;
			}
			symTabItems[i].numRef += 1;
			return 1;
		}
	}

	//printf(ANSI_COLOR_BRIGHT_YELLOW "\n ERROR: failed to set variable '%s' to '%s' in scope '%s' \n	REASON: -> not found in symbol table \n" ANSI_COLOR_RESET, itemName,  value, scope);
	return 0;
}

int TryDecrementVarNumRef(char itemName[50], char scope[50])
{
	// Lookup an identifier in the symbol table
	// what about scope?
	// return TRUE or FALSE
	// Later on, you may want to return additional information
	for(int i=0; i<100; i++)
	{
		int str1 = strcmp(symTabItems[i].itemName, itemName);
		int str2 = strcmp(symTabItems[i].scope,scope);
		if( str1 == 0 && str2 == 0)
		{
			if(symTabItems[i].numRef - 1 == 1)
			{
				symTabItems[i].is_dead = 1;
			}
			symTabItems[i].numRef -= 1;
			return 1;
		}
	}

	//printf(ANSI_COLOR_BRIGHT_YELLOW "\n ERROR: failed to set variable '%s' to '%s' in scope '%s' \n	REASON: -> not found in symbol table \n" ANSI_COLOR_RESET, itemName,  value, scope);
	return 0;
}

void PerformLivenessAnalysis()
{
	printf(ANSI_COLOR_BRIGHT_YELLOW "\n #####- Performing Liveness Analysis -##### \n" ANSI_COLOR_RESET);
	for(int i = 0; i < 100; i++)
    {
		//&& strcmp(symTabItems[i].itemName, "") == 1
		if(symTabItems[i].numRef == 1 || symTabItems[i].numRef == 0 && symTabItems[i].is_dead == 0 && strcmp(symTabItems[i].itemName, "") != 0)
		{
			printf(ANSI_COLOR_BRIGHT_GREEN "\n Variable '%s' was marked dead " ANSI_COLOR_RESET, symTabItems[i].itemName);
			symTabItems[i].is_dead = 1;
		}
    }
	printf(ANSI_COLOR_BRIGHT_YELLOW "\n\n #####- Analysis End -##### \n" ANSI_COLOR_RESET);
}

char* TryGetVarValue(char itemName[50], char scope[50])
{
    for(int i = 0; i < 100; i++)
    {
        int str1 = strcmp(symTabItems[i].itemName, itemName);
        int str2 = strcmp(symTabItems[i].scope, scope);
        if(str1 == 0 && str2 == 0)
        {
            return symTabItems[i].itemValue;
        }
    }

    return NULL; // Return NULL if not found
}


int IsVarDead(struct ASTNode* ast_node_ref)
{
	// Lookup an identifier in the symbol table
	// what about scope?
	// return TRUE or FALSE
	// Later on, you may want to return additional information
	for(int i=0; i<100; i++)
	{
		if(ast_node_ref == symTabItems[i].ast_node_ref)
		{
			return symTabItems[i].is_dead;
		}
	}

	//printf(ANSI_COLOR_BRIGHT_YELLOW "\n ERROR: failed to set variable '%s' to '%s' in scope '%s' \n	REASON: -> not found in symbol table \n" ANSI_COLOR_RESET, itemName,  value, scope);
	return 0;
}

struct ASTNode* TryGetASTNodeRef(char itemName[50], char scope[50])
{
	// Lookup an identifier in the symbol table
	// what about scope?
	// return TRUE or FALSE
	// Later on, you may want to return additional information
	for(int i=0; i<100; i++)
	{
		int str1 = strcmp(symTabItems[i].itemName, itemName);
		int str2 = strcmp(symTabItems[i].scope,scope);
		if( str1 == 0 && str2 == 0)
		{
			return symTabItems[i].ast_node_ref; // found the ID in the table
		}
	}
	return NULL;
}


void freeSymbolTableVariables() {
    for (int i = 0; i < symTabIndex; i++) {
        // Free any resources associated with each variable AST node 
        if (symTabItems[i].ast_node_ref != NULL) {
            printf("Freed variable: ");
			printf(symTabItems[i].itemName);
			printf("\n");
			free(symTabItems[i].ast_node_ref);
            symTabItems[i].ast_node_ref = NULL;
        }

    }
}



#endif
