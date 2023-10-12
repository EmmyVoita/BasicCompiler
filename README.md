# BasicCompiler

To Run: navigate to folder and make

Parser:

For assignment statement [IDENTIFIER EQ NonAssignExpr]
	- Get a reference to the ASTNode of the declared variable. 
		- If it is null print error message for use before declaration
	- If it has been assigned already just update the existing assignment node by getting a reference from the symbol table
	- If it has not be assigned already create an assignment node and add it to the symbol table


IDENTIFIER OPERATOR NonAssignExpr 

- constant propogation: get the value from the symbol table replace the identifier with its value
	- constant folding:perform the calculation here, and return just a constant node instead of a making a binaryexpression ASTNode

NUMBER OPERATOR NonAssignExpr 

- constant folding : same



After Parsing-> LivenessAnalysis.
	
	Normally dead variables are dealt with as constant propogation occurs, but the parser doesn't mark a variable as dead accuratley when it isn't used for anything. 
	So to deal with that situation, perform a pass after parsing.
	- if any variable has less has 0 or 1 references and it is not marked dead, then mark it dead


Write IRcode to file. 
- Recursivley traverse the ASTTree. If some variable is not dead, then add it to IRCode and MIPS code
