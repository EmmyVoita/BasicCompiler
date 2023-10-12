#ifndef AST_H
#define AST_H

//Abstract Syntax Tree Implementation
#include <string.h>
#include "ConsoleFormating.h" // Add this line

// Define the types of nodes your AST will have
typedef enum 
{
    NODE_BINARY_EXPRESSION,
    NODE_ASSIGNMENT,
    NODE_CONSTANT,
    NODE_VARIABLE,  // Represents both variable declarations and identifiers
    NODE_STMTLIST,
    NODE_VARIABLE_ASSIGNMENT,
    NODE_STMT,
    NODE_PROGRAM,
    NODE_BLOCKLIST,
    NODE_WRITE

} NodeType;

// Define the possible operators for binary operations
typedef enum 
{
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
} OperatorType;

// Define the AST node struct
struct ASTNode
{
    NodeType node_type;    // a field in the struct AST that is used to represent the type or category of the node (e.g., )
    OperatorType operator; 

    char *tempVar;
    struct ASTNode* left;    // Left child node
    struct ASTNode* right;   // Right child node

    char value[50];
    char type[50];
};

struct ASTNode* createVariableDeclarationNode(char name[50], char type[50]) 
{
    struct ASTNode* node = malloc(sizeof(struct ASTNode));      
   
    // Set the type of the node
    node->node_type = NODE_VARIABLE;

    strcpy(node->type, type);
    strcpy(node->value, name);
    
    return node;
}

struct ASTNode* createBinaryExpressionNode(char operator[50], struct ASTNode* left, struct ASTNode* right) 
{
    struct ASTNode* node = malloc(sizeof(struct ASTNode));      
   
    // Set the type of the node
    node->node_type = NODE_BINARY_EXPRESSION;

    strcpy(node->value, operator);

    node->left = left;
    node->right = right;
    
    return node;
}


struct ASTNode* createConstantNode(char value[50]) 
{
    struct ASTNode* node = malloc(sizeof(struct ASTNode));

    node->node_type = NODE_CONSTANT;

    strcpy(node->value, value);

    return node;
}


struct ASTNode* createNode(struct ASTNode* left, struct ASTNode* right, NodeType node_type) 
{
    struct ASTNode* node = malloc(sizeof(struct ASTNode));      
   
    // Set the type of the node
    node->node_type = node_type;

    node->left = left;
    node->right = right;
    
    return node;
}




void printAST(struct ASTNode* node, int level, int right)
{
    if (node == NULL)
        return;
    

    // Print indentation based on the level
    for (int i = 0; i < level-1; i++)
        printf("     ");  
    if (right == 1)
        printf(ANSI_COLOR_CYAN" └── ""\x1b[0m");
    else
        printf(ANSI_COLOR_CYAN" ├── ""\x1b[0m");
        
    switch (node->node_type)
    {
        
        case NODE_PROGRAM:
            printf(ANSI_COLOR_BRIGHT_CYAN "Program: \n" ANSI_COLOR_RESET);
            break;
        case NODE_VARIABLE:
            printf(ANSI_COLOR_BRIGHT_MAGENTA "Variable: " ANSI_COLOR_RESET);
            printf(ANSI_COLOR_BRIGHT_CYAN  "%s (%s)\n" ANSI_COLOR_RESET, node->value, node->type);
            break;
        case NODE_VARIABLE_ASSIGNMENT:
            printf( ANSI_COLOR_BRIGHT_MAGENTA "Variable: " ANSI_COLOR_RESET);
            printf( ANSI_COLOR_BRIGHT_CYAN  "%s \n" ANSI_COLOR_RESET, node->value);
            break;
        case NODE_STMTLIST:
            printf("STMTLIST: \n");
            break;
        case NODE_WRITE:
            printf("NODE_WRITE: \n");
            break;
        case NODE_STMT:
            printf(ANSI_COLOR_BRIGHT_WHITE "Statement: \n" ANSI_COLOR_RESET);
            break;
        case NODE_BLOCKLIST:
            printf(ANSI_COLOR_BRIGHT_WHITE "NODE_BLOCKLIST: \n" ANSI_COLOR_RESET);
            break;
        case NODE_BINARY_EXPRESSION:
            printf(ANSI_COLOR_BRIGHT_MAGENTA "Binary Expression: %s" ANSI_COLOR_RESET, node->value);
            break;
        case NODE_CONSTANT:
            printf(ANSI_COLOR_BRIGHT_MAGENTA "Constant: " ANSI_COLOR_RESET);
            printf(ANSI_COLOR_BRIGHT_CYAN  "%s\n" ANSI_COLOR_RESET, node->value);
            break;
        case NODE_ASSIGNMENT:
            printf(ANSI_COLOR_BRIGHT_MAGENTA "Assignment: " ANSI_COLOR_RESET);
            printf(ANSI_COLOR_BRIGHT_CYAN  "= \n" ANSI_COLOR_RESET);
            break;
        default:
            printf("Unknown Node Type\n");
            break;
    }

    // Recursively print left and right nodes with increased indentation level
    printAST(node->left, level + 1, 0);
    printAST(node->right, level + 1, 1);
}


void freeAST(struct ASTNode* node) 
{
    
    if (node == NULL) {
        return;
    }

    // Recursively free left and right child nodes
    freeAST(node->left);
    freeAST(node->right);
    
    // Free any resources specific to this node
    if (node->node_type != NODE_VARIABLE) 
    {
        // Example: If NODE_VARIABLE has dynamically allocated fields
        // free(node->tempVar);
        
        
        // Free the node itself
        free(node);
    }

    
}

#endif








































