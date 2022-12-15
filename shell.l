
/*
 *
 * CS-413 Spring 98
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <string.h>
#include "y.tab.h"

%}

%%

\n 	{
		return NEWLINE;
	}

[ \t] 	{
		/* Discard spaces and tabs */
     	}
     		
">>" 	{
		return GREATER;
	}
	
"|" 	{
		return PIPER;
	}
	
"<" 	{
		return LESS;
	}		

">" 	{
		return GREAT;
	}
	
"&" 	{
		return AMPERSAND;
	}
	
">&"	{
	return GREATAMPERSAND;	
	}
">>&"	{
	return GREATERAMPERSAND;	
	}	
	
"exit" 	{
		return EXIT;
	}
"cd"	{
		return CD;
	}
	
"^.${[^}]+}.$" {
		return SPECIAL;
	}			
	

"[^ \t\n][^ \t\n]*"  {
		/* Assume that file names have only alpha chars */
		yylval.string_val = strdup(yytext);
		return WORD;
	}
[^\|\>\<\& \t\n][^\|\>\<\& \t\n]*  {
   		 yylval.string_val = strdup(yytext);
   		 return WORD;
		}	
.	{
		/* Invalid character in input */
		return NOTOKEN;
	}

%%
	
