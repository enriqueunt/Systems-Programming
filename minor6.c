//Enrique Torres
//DESCRIPTION: This program would create two child process for command line arguments parent wait unitl childs finishes
//DUE Date: 11/7/16
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

//minor 6
int main(int argc, char *argv[]){
  int fd[2];//file ddescriptor
  //  int fd2[2];
    int i;
    const int READ = 0; //read is 0
    const int WRITE = 1; // write is 1
    const int STDIN = 0; //stdin is 0
    const int STDOUT = 1;// stdout is 1
    //    char line[80];
    char *argv2[20]; //temp char pointer array
    char *argv1[20]; 
    
    
    if (argc < 3 || argc > 3 ){// if statement if argc is less than three or greater than 3
    printf("error: too few or too many arguments: %d\n", argc);
  }
    else { // else run the program
    printf("executing: %s | %s \n",argv[1],argv[2]);
    if (pipe(fd) == -1) //error handling for pipe
      {
	perror("Pipe");
	exit(1);
      }
    
    //if (fork() == -1) //error handling for fork
	// {
    //	perror("Fork");
    //	exit(1);
    // }
    else if (fork() == 0) //for creating the first child
      { //in child1
	
	close(STDOUT);
	dup2(fd[WRITE], fileno(stdout)); 
	close(fd[READ]);
	close(fd[WRITE]);
	argv1[0]=argv[1];
	execvp(argv1[0], argv1);
	  //int execvp(const char *file, char *const argv[]);
	exit(1);//exits child
      }
    //    int pd2 = fork();
    else if (fork() == 0) //for creating the second child
	  { //child2
	    wait(NULL);
	    close(STDIN);
	    dup2(fd[READ], fileno(stdin));
	    close(fd[WRITE]);
	    close(fd[READ]);
	    argv2[0]=argv[2];
	    execvp(argv2[0], argv2);
	    //    perror("execvp of wc failed");
	    //int execvp(const char *file, char *const argv[]);
	    exit(1);//exits child

	  }
    
    else
	  {//parent
	    close(fd[READ]);//closing read
	    close(fd[WRITE]);//closing write
	    wait(NULL);//waiting for child process to end
	    wait(NULL);
	    printf("command line pipe completed\n");
	    exit(1); //exits
	    //printf("command line pipe completed\n");
	  }
    
  }
  

  return 0;
}
