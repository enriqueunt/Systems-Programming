/* Enrique Torres
   CSCE 3600: Minor 1
   Due Date 9/5/2016
   Details: This program would take the a ASCII value from the user 
   and convert it to a decimal, hexadecimal, octal, binary
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){

  char input;
  int i;
  int j;
  int k;
  int z;
  int integer;
  // int binary[10];
  printf ("Enter an ASCII character: ");
  scanf("%c",&input);
  printf("The ASCII value of %c is: \n", input);
   integer =  input ; 
   printf("dec -- %d\n",integer); //%d prints the decimal of the caharacter
   printf("hex -- %x\n",integer); //%x prints the hex format of the integer
   printf("oct -- %o\n",integer); //%o prints the octal format of the integer

   printf("bin -- "); //printing the binary
   for ( i = 7; i >= 0; i--){ //making the result 8 bits long
    j = integer >> i;
    if (j & 1) // using bolean 
      printf("1");
    else
      printf("0");
  }
  printf("\n");

  return 0;
}
