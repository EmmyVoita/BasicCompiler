%option noyywrap

%{
#include <stdio.h>
#include <string.h>

#define YY_DECL int yylex()
#include "parser.tab.h"

int words = 0;
int chars = 0;
int lines = 0;

%}

LETTER      [a-zA-Z]
DIGIT       [0-9]
OPERATOR	[+\-*/%]
IDENTIFIER  {LETTER}({LETTER}|{DIGIT})*
delim       [ \t\n]
NUMBER      {DIGIT}+(\.{DIGIT}+)?(E[+\-]?(DIGIT)+)?
ws          {delim}+

%%
"/*"    				{
							int c;
							while((c = input()) != 0) {
								if(c == '*') {
									if((c = input()) == '/')
										break;
									else
										unput(c);
								}
							}
						}


"int"			{words++; chars += strlen(yytext);
					printf("%s : TYPE\n", yytext);
					yylval.string = strdup(yytext); 
					return TYPE;
				}


"write"			{words++; chars += strlen(yytext);
					printf("%s : KEYWORD\n", yytext);
					yylval.string = strdup(yytext); 
					return WRITE;
				}

{IDENTIFIER}	{words++; chars += strlen(yytext);
					printf("%s : IDENTIFIER\n",yytext);
					yylval.string = strdup(yytext); 
					return IDENTIFIER;
				}
			
{NUMBER}		{words++; chars += strlen(yytext);
				printf("%s : NUMBER\n",yytext);
				yylval.number = atoi(strdup(yytext)); 
				return NUMBER;
				}

{OPERATOR}		{words++; chars += strlen(yytext);
					printf("%s : OPERATOR\n",yytext);
					yylval.string = strdup(yytext); 
					return OPERATOR;
				}

		
";"				{chars++;
					printf("%s : SEMICOLON\n", yytext);
					yylval.string = strdup(yytext); 
					return SEMICOLON;
				}
				
"{"				{chars++;
					printf("%s : LCBRA\n", yytext);
					yylval.string = strdup(yytext); 
					return LCBRA;
				}

"}"				{chars++;
					printf("%s : RCBRA\n", yytext);
					yylval.string = strdup(yytext); 
					return RCBRA;
				}				
		
"="				{chars++;
					printf("%s : EQ\n", yytext);
					yylval.string = strdup(yytext); 
					return EQ;
				}




[\n]					 {lines++; chars=0;}
[\t ]+                   {chars++; } 
.		{	
			chars++;
        	printf("%s : Unrecognized symbol at line %d char %d\n", yytext,lines,chars);
		}
		
%%