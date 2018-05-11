/*Enrique Torres
 *CSCE 3600: Minor 2
 *Due Date 09/21/16
 *Description: This assignment parse the command the user inputs and outputs the systemcall
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printArguments(int argc, char *argv[]);

int main(int argc, char **argv)
{
  printArguments(argc, argv);
  return 0;

}
void printArguments(int argc, char *argv[])
{
  int i;
  char buffer[80];
  int number;
  char dummy[80];
  int j;
  char mystring[80];
  char dummy2[80];
  // for (i = 0; i < 80; i++) //nulling my strings
  // {
  //   buffer[i] ='\0';
  // }
  for (i = 0; i < 80; i++){ // nulling my string
    buffer[i] = '\0';
  }
  for (i = 0; i < 80; i++) // nulling my string
    {
      dummy[i] = '\0';
    }
  

  strcpy(buffer, "du -a "); //setting my buffer to be command "du -a"
 
  for (i = 1; i < argc; i++)
    {
      if (argv[i][0] == '-'){
       switch (argv[i][1])
	 {
	 case 'd':
	   
	   for (j = 2; j < strlen(argv[i]); j++){ // similar to the h case

	     mystring[j-2] = argv[i][j];
	   }
	  
	   strcpy(buffer+(strlen(buffer)),mystring); 
	   
	   break;


	 case 's':
	   
	   strcat(buffer, " "); // concatenate the sort -n -r for the system buffer
	    strcat(buffer, "|");
	    strcat(buffer, " ");
	    strcat(buffer, "sort -n -r ");
	     
	    break; //end of case
	 case 'h':     
	   strcat(buffer, "| head -n "); // placing head -n in the buffer
	     
	   for (j = 2; j < strlen(argv[i]); j++) // finding the number after the the head -n
	       {

		 dummy[j-2] = argv[i][j]; // placing what is after the -n in a dummy string
	     }
	   strcpy(buffer+(strlen(buffer)), dummy); // copying the dummy string in the buffer.

    
	     break; //end of case
	 case 'o':
	   strcat(buffer, " > " ); // adding the > in the buffer

	   for (j = 2; j < strlen(argv[i]); j++){ // similar to the h case

	    dummy[j-2] = argv[i][j];

	    }
	    strcpy(buffer+(strlen(buffer)), dummy);

	    break; // end of case

	 default:

	   // system(buffer);
	   printf("Unknown flag: %s\n", argv[i]); // prints the unknow instruction
	   printf("Program terminated...\n");
	   exit(1); // end the program
	 }
     
      }
    }
  printf("cmd: %s\n",buffer);
  system(buffer);

}
