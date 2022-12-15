/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fstream>
#include "command.h"
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

		
time_t t;
		  
using namespace std;
SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}
	
	_arguments[ _numberOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;
	
	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_greaterFlag=0;
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

	if ( _outFile ) {
		free( _outFile );
	}

	if ( _inputFile ) {
		free( _inputFile );
	}

	if ( _errFile ) {
		free( _errFile );
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_greaterFlag=0;
}

void
Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
			
		
		}
	
	}
      
	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "   %-12s   %-12s   %-12s   %-12s\n", _outFile?_outFile:"default", _inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO");
	printf( "\n\n" );
	
}
void handler(int signo)
{     
                      time(&t);
       			ofstream fw ("child.log",std :: ios_base :: app);
       			if (fw.is_open()){
       				fw <<"child pid done at:"<<ctime(&t)<<"\n";
       				}
       			fw.close();  
    }
    
    
    
    

void
Command::execute()
{
	// Don't do anything if there are no simple commands
	if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}

	// Print contents of Command data structure
	
        print();
	// Add execution here
	// For every simple command fork a new process
	// Setup i/o redirection
	// and call exec
		int defaultin = dup( 0 );
		int defaultout = dup( 1 );
		int defaulterr = dup( 2 );
		// Create file descriptor 
		int outfd,infd,errfd;
		int pid;
		
		if( _inputFile != 0)
		 {
			 if ( infd < 0 ) {
		 		perror( "error in infile" );
		 		exit( 2 );
					}
		 	infd=open ( _inputFile ,O_RDONLY );
		 	}
		 else
		  {
		 
		 	infd=dup(defaultin);
		  }
		   if( _errFile != 0){
		            if (_greaterFlag)
		            {
				errfd=open ( _errFile ,O_CREAT|O_TRUNC|O_WRONLY,S_IRWXU |S_IRWXG);
						}
        			else
       					{
       				errfd=open ( _errFile ,O_CREAT|O_APPEND|O_WRONLY,S_IRWXU |S_IRWXG);
					}
		   }
		  else
				{
				errfd=dup(defaulterr);
				  } 
		
		for ( int i=0; i<_numberOfSimpleCommands ; i++ ){
			dup2(infd,0);
			close (infd);
			dup2(errfd,2);
			close (errfd);
			if (i==_numberOfSimpleCommands -1){
		
				if( _outFile != 0){
					if ( outfd < 0 ) {
						perror( "error in create outfile" );
		 				exit( 2 );
					                 }
					else
					      {	
						if (_greaterFlag){
							outfd=open ( _outFile ,O_CREAT|O_TRUNC|O_WRONLY,S_IRWXU |S_IRWXG);
							}
						else
						{
							outfd=open ( _outFile ,O_CREAT|O_APPEND|O_WRONLY,S_IRWXU |S_IRWXG);
							}
		
					         }
			                		        
					         }
				
					              			
		                    	         
		                else
		                {
		
					outfd=dup(defaultout);
		         	}
		         	
				}
			else
			{
		
				int fdpipe[2];
				pipe(fdpipe);
				outfd=fdpipe[1];
				infd=fdpipe[0];
		         }
			
			dup2(outfd,1);
			
			close(outfd);
			//dup2(errfd,2);
			//close(errfd);
			pid=fork();
			if(pid ==0)
			{
			
       				execvp(_simpleCommands[i]->_arguments[ 0] ,_simpleCommands[i]->_arguments);
       				return;
       				perror("execvp");
       				exit(0);
       			}
        		else if (pid < 0)
        		{
        			printf( "Forking child failed" );
        			exit(0);
       			}
       		    }
       		
       		dup2( defaultin, 0 );
		dup2( defaultout, 1 );
		dup2( defaulterr, 2 );

	// Close file descriptors that are not needed
		//close( outfd );
		//close( infd );
		//close (errfd);
		close( defaultin );
		close( defaultout );
		close( defaulterr );
       		
       		//signal_handler(SIGINT,logger);
       		signal(SIGINT,SIG_IGN);
       		 
       		if(!_background)
       		{	
       			waitpid( pid,0,0);
       			
       		
       		}
       		
		
	// Clear to prepare for next command
	clear();
	
	// Print new prompt
	prompt();
}



// Shell implementation

void
Command::prompt()
{
	printf("myshell>");
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

int yyparse(void);

int 
main()
{	signal(SIGINT,SIG_IGN);
	signal(SIGCHLD,handler);
	Command::_currentCommand.prompt();
	yyparse();
	return 0;
}
