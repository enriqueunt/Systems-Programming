#!/bin/bash

#Enrique Torres
#Minor 3: Due 09/28/16
#CSCE 3600
#Description: This bash program will display a menu allowing the user to enter an option
#option1 counts the nunber of files and displays the files
#option2 runs the process of the user
#option3 finds the factors of a number
#option4 exits the program


 if [[ $# -ge 1 && $# -le 2 ]];
  then
     
 echo "Good day, $1! Nice to meet you!"
  
while true; do #continous loop for the menu            
 echo "+*******************************************************************+"
 echo " Enter one of the following options:                                |"
 echo "1) List and count all non-hidden files in the current directory.    |"
 echo "2) Check if given user (default = curren user) is logged in, then   |"
 echo "   ..list all active processes for the user.                        |"
 echo "3) Display prime factors of user-entered positive integer.          |"
 echo "4) Exit this shell script.                                          |"
 echo "+*******************************************************************+"
 
 read -p "> " options #reads in input


  case $options in #case state starts
     1) 
	  echo -n "Total number of files: "
	  
	  ls -1 | wc -l #counts the number of files
	  ls            #prints the list of files
          ;;

     2)
	 if [ $# -eq 2 ]; then #if the number of command arguments equal to 2 then user is the position 2 which is the unt id
	     USER=$2    

	 else 
	     USER=`whoami` #else user is whoami which is the user id
	 fi
	 
	 check=`who | grep $USER` #searches for the occurrence of a string 
	 
	 if [ -n "$check" ];then 
	     echo "==> Active process for $USER (logged in): "
	     ps U $USER #prints the process
	     else 
	     echo "==> Active process for $2 (NOT logged in): "
	     ps U $2
	     fi
	     ;;
      
     3)
	  echo "==> Display prime factors of user-enter integer: "
          read -p "Enter a positive integer to find its prime factors: " number #enter number
	  echo `factor $number` #factor fuction to perform math
	  ;;

     4)
	  echo "==> Exit this shell script:" #exits the shell script
	  echo "Thanks, $1! Have a great day!"
	  exit 0
	  ;;

     *)   echo "Invalid option. Please select again!" 
	  ;;
 

  esac
  done
 else 
   echo "usage: minor3 name [euid]"
 fi
