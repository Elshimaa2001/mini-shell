/*
 * CS-413 Spring 98
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%token	<string_val> WORD

%token 	NOTOKEN GREAT GREATER PIPER NEWLINE LESS AMPERSAND EXIT SPECIAL GREATAMPERSAND GREATERAMPERSAND CD

%union	{
		char   *string_val;
	}

%{
extern "C" 
{
	int yylex();
	void yyerror (char const *s);
}
#define yylex yylex
#include <stdio.h>
#include "command.h"
#include <unistd.h>
%}

%%

goal:	
	commands
	;

commands: 
	command
	| commands command 
	;

command: simple_command
        ;

simple_command:	
	pipe_list iomodifier_opt_list background_opt NEWLINE {
		printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();
	}
	| NEWLINE 
	| error NEWLINE { yyerrok; }
	| EXIT NEWLINE {
		exit(0);
	} 
	| CD WORD NEWLINE{
		char ss[100];
		printf("your directory was %s\n",getcwd(ss,100));
		chdir($2);
		printf("current directory is %s\n",getcwd(ss,100));
	}
	| CD NEWLINE{
	char ss[100];
		printf("your directory was %s\n",getcwd(ss,100));
		chdir("/home");
		printf("current directory is %s\n",getcwd(ss,100));
	
	}
	;

command_and_args:
	command_word arg_list {
		Command::_currentCommand.
			insertSimpleCommand( Command::_currentSimpleCommand );
	}
	;
	
pipe_list:
	pipe_list PIPER	command_and_args{
	}
	| command_and_args
	;
	

arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
	WORD {
               printf("   Yacc: insert argument \"%s\"\n", $1);

	       Command::_currentSimpleCommand->insertArgument( $1 );\
	}
	;

command_word:
	WORD {
               printf("   Yacc: insert command \"%s\"\n", $1);
	       
	       Command::_currentSimpleCommand = new SimpleCommand();
	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	
	;
iomodifier_opt_list:
	iomodifier_opt_list iomodifier_opt
	|/* can be empty */
	;
iomodifier_opt:
	GREAT WORD {
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand._greaterFlag=1;
	}
	
	|   GREATAMPERSAND WORD {
		
		//Command::_currentCommand._outFile = $2;
		Command::_currentCommand._errFile = $2;
		Command::_currentCommand._greaterFlag=1;
	
	}
	
	| GREATER WORD {
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		
	}
	|GREATERAMPERSAND WORD  {
	
		//Command::_currentCommand._outFile = $2;
		Command::_currentCommand._errFile = $2;
	
	
	}
	
	
	| LESS WORD {
		printf("   Yacc: insert input \"%s\"\n", $2);
		Command::_currentCommand._inputFile = $2;
	}
	
	;
	
	
background_opt: 
	AMPERSAND {
		Command::_currentCommand._background=1;
	}	
	|/* can be empty */
	;
	

%%

void
yyerror(const char * s)
{
	fprintf(stderr,"%s", s);
}

#if 0
main()
{
	yyparse();
}
#endif
