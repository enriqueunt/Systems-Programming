

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

// Definitions
#define STACK_SIZE 15
#define STACK_EMPTY -1
#define REQUESTS 15

// Ticket Stack and top identifier
int s[STACK_SIZE];
int s_top = STACK_EMPTY;

// Pushes items onto stack
void push(int *stack, int item, int *top, int max_size) {
    stack[++(*top)] = item;
}

// Pops items from stack
int pop(int *stack, int *top) {
    // Return zero if empty stack
    if (*top < 0) {
        return 0;
    }
    // Otherwise, return value and clear out other values
    else {
		int num = stack[(*top)--];
        stack[(*top)+1] = 0;
		return num;
    }
}

// Error function periodically called by program
void error(const char *msg) {
    perror(msg);
    exit(0);
}

// Main function
int main(int argc, char *argv[]) {

    // Variables that hold connection info, and counters
    int sockfd, portno, n, i, count;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];   // String buffer used for read/write
    if (argc < 3) {
       fprintf(stderr,"ERROR: USAGE: %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]); // Convert string to int

    // Define socket file descriptor
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Error check
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    // Define server and append the rest of the hostname to it.
    server = gethostbyname(strcat(argv[1],".cse.unt.edu"));

    // Error check
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));  // Clear out connection variables
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    // Connect to server + Error check
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }

    // START GENERATING REQUESTS:

	// BUY TICKETS - 10 times
    for (count = 0; count < 11; count++) {

        bzero(buffer,256);      // Clear out buffer
        strcpy(buffer, "BUY");  // Request is BUY

        // Write to socket
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) {
            error("ERROR writing to socket");
        }

        bzero(buffer,256);  // Zero out buffer to use for read operation
        n = read(sockfd,buffer,255);    // Read message from server

        // Error check
        if (n < 0) {
            error("ERROR reading from socket");
        }

        // Print appropriate message
        printf("[CLIENT]: BUY\n");
        printf("[SERVER]: %s\n",buffer);

        // Database is full error check
        if (strcmp(buffer, "Database Full") == 0) {
            printf("[CLIENT]: Buy Failed.\n");
        }
        int val = atoi(buffer);     // Convert string to int
        push(s, val, &s_top, STACK_SIZE);   // Push newly received ticket ID onto stack
    }

	// RETURN TICKETS - 2 times
    for (count = 0; count < 2; count++) {

        bzero(buffer,256);  // Zero out buffer
        strcpy(buffer, "RETURN ");  // Request is RETURN
		int returnID = pop(s, &s_top);    // Pop a new ticket id off the stack

        // Of the returned ID is zero, keep popping to get an existing ID
        while (returnID == 0) {
			returnID = pop(s, &s_top);
		}
        char returnIDString[5]; // Define string to hold id
        sprintf(returnIDString, "%d", returnID);    // Convert int id to string
        strcat(buffer, returnIDString); // Add ID to buffer
        printf("[CLIENT]: %s\n",buffer);    // Print message

        // Write buffer to server
        n = write(sockfd,buffer,strlen(buffer));

        // Error check
        if (n < 0) {
            error("ERROR writing to socket");
        }
        bzero(buffer,256);  // Clear out buffer for read operation
        n = read(sockfd,buffer,255);    // Read from server

        // Error check
        if (n < 0) {
            error("ERROR reading from socket");
        }

        // Invalid return
        if (strcmp(buffer, "NO") == 0) {
            printf("[SERVER]: Ticket not exist.\n");
            printf("[CLIENT]: Return failed.\n");
            push(s, returnID, &s_top, STACK_SIZE);  // Push ID back onto stack
        }

        // Valid return
        else {
            printf("[SERVER]: %s\n",buffer);
            printf("[CLIENT]: Ticket Returned.\n");
        }
    }

	// RETURN NONEXISTENT TICKET - 1 time - *** SAME COMMENTS AS PREVIOUS BLOCK ***
	for (count = 0; count < 1; count++) {

        bzero(buffer,256);
        strcpy(buffer, "RETURN 55555");
        printf("[CLIENT]: %s\n",buffer);
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) {
            error("ERROR writing to socket");
        }
        bzero(buffer,256);
        n = read(sockfd,buffer,255);
        if (n < 0) {
            error("ERROR reading from socket");
        }

        if (strcmp(buffer, "NO") == 0) {
            printf("[SERVER]: Ticket not exist.\n");
            printf("[CLIENT]: Return failed.\n");
        }
        else {
            printf("[SERVER]: %s\n",buffer);
            printf("[CLIENT]: Ticket Returned.\n");
        }
    }

	// BUY TICKETS - 2 times - *** SAME COMMENTS AS PREVIOUS BLOCK ***
    for (count = 0; count < 2; count++) {

        // Buy
        bzero(buffer,256);
        strcpy(buffer, "BUY");

        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) {
            error("ERROR writing to socket");
        }

        bzero(buffer,256);
        n = read(sockfd,buffer,255);
        if (n < 0) {
            error("ERROR reading from socket");
        }
        printf("[CLIENT]: BUY\n");
        printf("[SERVER]: %s\n",buffer);
        if (strcmp(buffer, "Database Full") == 0) {
            printf("[CLIENT]: Buy Failed.\n");
        }
        int val = atoi(buffer);
        push(s, val, &s_top, STACK_SIZE);
    }

	// SHOW DATABASE
    printf("[CLIENT]: Database Table:\n");
    for(i=0; i<=14; i++){
        printf("[%d]  %d\n", i, s[i]);
    }
    printf("\n");
    printf("[CLIENT]: FINISHED.\n");

    close(sockfd);  // Close connection
    return 0;
}
