
#ifndef PRINT_RECURS
#define PRINT_RECURS 0
#endif


#ifndef IR_CODE_H
#define IR_CODE_H
// ---- Functions to handle IR code emissions ---- //

#include "AST.h" 
#include "ConsoleFormating.h"
#include "MIPScode.h"



const char* OperatorStrings[] = 
{
   "+",
   "-",
   "*",
   "-"
};

int tempVarCounter = 0;

char* generateNewTempVar() 
{
    char *tempVar = (char*) malloc(10);
    snprintf(tempVar, 10, "t%d", ++tempVarCounter);
    return tempVar;
}

void ManageBinaryExpressionNode(struct ASTNode *node)
{
    char *newTemp = generateNewTempVar();
    char currentIRLine[100], currentMIPSLine[100];



    sprintf(currentIRLine, "%s = %s %s %s\n", 
    newTemp,
    node->left->tempVar ? node->left-> tempVar :  node->left->value, // Replace "..." with actual value or var name
    node->value,
    node->right->tempVar ? node->right->tempVar : node->right->value // Replace "..." with actual value or var name
    );

    push(currentIRLine);
    printf(currentIRLine);

    /*sprintf(currentMIPSLine, "li $%s, %s \n", 
    newTemp,
    node->right->tempVar ? node->right->tempVar : node->right->value // Replace "..." with actual value or var name
    );
    printf(currentMIPSLine);*/

    node->tempVar = newTemp;
}

void ManageAssignmentNode(struct ASTNode *node)
{
    // This is a ternary conditional expression (also known as the conditional operator ?:).
    // It checks if node->left->tempVar is non-null (i.e., it has a value). If true, it returns node->left->tempVar, otherwise, it returns "..." (three dots).


    char *newTemp = generateNewTempVar();
    char currentIRLine[50], currentMIPSLine[50];


    if(IsVarDead(node->left) == 0)
    {
        sprintf(currentIRLine, "%s = %s \n", newTemp, node->right->tempVar ? node->right->tempVar : node->right->value );
        push(currentIRLine);
        printf(currentIRLine);

        sprintf(currentMIPSLine, "li $%s, %s \n", newTemp, node->right->tempVar ? node->right->tempVar : node->right->value);
        pushMIPS(currentMIPSLine);
    }
    else if(IsVarDead(node->left) == 1)
    {
        printf(ANSI_COLOR_BRIGHT_GREEN  "Dead : %s = %s \n" ANSI_COLOR_RESET, newTemp, node->right->tempVar ? node->right->tempVar : node->right->value);
    }
    
    node->left->tempVar = newTemp;
   
}

void ManageWriteNode(struct ASTNode *node)
{

    char *newTemp = generateNewTempVar();
    char currentIRLine[50], currentMIPSLine[100];

    sprintf(currentIRLine, "print = %s \n", node->right->tempVar ? node->right->tempVar : node->right->value );

    push(currentIRLine);
    printf(currentIRLine);
    
    sprintf(currentMIPSLine, "move $a0, $%s \n", node->right->tempVar ? node->right->tempVar : node->right->value);

    pushMIPS(currentMIPSLine);
    pushMIPS("li $v0, 1 \n");
    pushMIPS("syscall \n");

    node->tempVar = newTemp;
}

void printIRCode(struct ASTNode *node) 
{
    if (node == NULL) return;

    #if PRINT_RECURS == 1
        // Add printf statements here to track the flow
        printf("Visiting node of type: %s\n", EnumStrings[node->node_type]);
    #endif

    // Traverse the left and right children
    printIRCode(node->left);
    printIRCode(node->right);
    

    switch (node->node_type)
    {
    case NODE_ASSIGNMENT:
        ManageAssignmentNode(node);
        break;
    case NODE_BINARY_EXPRESSION:
        ManageBinaryExpressionNode(node);
        break;
    case NODE_WRITE:
        ManageWriteNode(node);
        break;
    default:
        //printf("Unknown Node Type\n");
        break;
    }
}


void WriteIRCodeToFile(struct ASTNode *node)
{
    
    FILE *file,*mips_file;


    // "w" means open for writing (creates if not exist)
    file = fopen("IRCode.ir", "w"); 
    mips_file = fopen("MipsScript.s", "w");
    
    if (file == NULL || mips_file == NULL ) {
        printf("Error opening IR.ir or MIPS.s file.\n");
        return;
    }

    clearStack();
    clearMIPS();

    printIRCode(node);

    printStackContentsToFile(file); 
    fclose(file);

    printMIPSStackContentsToFile(mips_file);
    fclose(mips_file);
}



#endif