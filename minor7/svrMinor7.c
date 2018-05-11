

// Libraries
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <pthread.h>

// Definitions
#define STACK_SIZE 20
#define STACK_EMPTY -1
#define REQUESTS 15

int s[STACK_SIZE];          // Ticket stack
int p[STACK_SIZE];          // Purchased stack
int s_top = STACK_EMPTY;    // Top for ticket stack
int p_top = STACK_EMPTY;    // Top for purchased stack
int clientID = 0;           // Keeps track of number of clients
int threadID = 0;           // Keeps track threads
int threadStatus = 0;       // Is the thread done?

// Thread Info - Passed into worker function
struct info {
    int sock;   // Sock fd
    int empty;  // No more tickets
};

// Pushes items into various stacks
void push(int *stack, int item, int *top, int max_size) {
    stack[++(*top)] = item;
}

// Pops items from various stacks
int pop(int *stack, int *top) {
    // Return 0 if no items
    if (*top < 0) {
        return 0;
    }
    // Otherwise return the item, and clear out other items in stack
    else {
		int num = stack[(*top)--];
        stack[(*top)+1] = 0;
		return num;
    }

}

// Error function used throughout
void error(const char *msg) {
    perror(msg);
    exit(1);
}

// Worker function that processes all the requests for each thread
void *thread_worker(void *data) {
    struct info *info = data;   // Info struct passed into thread
    int n;      // Counter variable
    int i;      // Counter variable
    int count;  // Counter variable
    int numid;  // Store ID as integer
    int sock = info->sock;  // Stores sock fd int
    int cliID = ++clientID; // Sets each client's specific ID and increments it.

    char buffer[256];   // String buffer used throughout read/write
    char id[5];         // Store ID as string

    // If first thread, display connected message.
    if (threadID == 1) {
        printf("[Client 1]: Connected...\n");
        printf("[SERVER X]: Waiting for additional client connections...\n");

    }
    while (threadID < 2) {} // Infinite wait for another client to connect.

    // Loop through all requests
    for (count = 0; count <= REQUESTS; count++) {

        usleep(600000); // Delay in execution - for visibility

        bzero(buffer,256);  // Clear out buffer
        n = read(sock,buffer,255);  // Read from socket

        // Appropriate error message
        if (n < 0) {
           error("ERROR reading from socket\n");
           exit(0);
        }

        // Request = BUY
        if (strcmp(buffer, "BUY") == 0) {
           numid = pop(s, &s_top);      // Get new ticket number from stack
           sprintf(id, "%d", numid);    // Convert to string

           // If ticket number is not zero
           if (numid != 0) {

               // Print out the appropriate message, push onto the stack, and write to the client.
               printf("[CLIENT %d]: %s\n", cliID, buffer);
               printf("[SERVER X]: Client %d buy: %s\n", cliID, id);
               push(p, numid, &p_top, STACK_SIZE);  // Add to purchased stack
               n = write(sock, id, 255);

               // Error check
               if (n < 0) {
                   error("ERROR writing to socket\n");
                   exit(0);
               }
           }

           // If ticket number is zero
           else {
               // Set empty flag, display message, write to socket telling client that database is full.
               info->empty = 1;
               printf("[CLIENT %d]: %s\n", cliID, buffer);
               printf("[SERVER X]: Database Full\n");
               n = write(sock, "Database Full", 255);
               // Error check
               if (n < 0) {
                   error("ERROR writing to socket");
                   exit(0);
               }
           }
        }

        // Request = RETURN
        else if(strncmp(buffer, "RETURN ", 7) == 0) {
            int x;                  // Counter variable
            char returned[5];       // Stores return ticket number as string
            bzero(returned,5);      // Clear it out
            // Store return ID into returned
            for (x=0; x < 5; x++) {
                returned[x] = buffer[x+7];
            }
            // Print appropriate message
            printf("[CLIENT %d]: %s\n", cliID, buffer);
            int val = atoi(returned);   // Convert string into int
            int valid = 0;  // Set valid flag
            // Run through past tickets, and make sure there is a match.
            for(i=p_top; i>=0; i--){
                if (p[i] == val) {
                    valid = 1;
                    break;
                }
                else {
                    valid = 0;
                }
            }

            // If return is valid, process it, push it onto stack, and write success message to client.
            if (valid == 1) {
                printf("[SERVER X]: Client %d cancel %s\n", cliID, returned);
                push(s, val, &s_top, STACK_SIZE);
                n = write(sock, buffer, 255);
            }

            // Otherwise, tell client the return is invalid, and do nothing.
            else {
                printf("[SERVER X]: Client %d cancel %s failed.\n", cliID, returned);
                n = write(sock, "NO", 255);
            }
        }
        else {
			// Do nothing.  Ignore incorrect request.
		}
    }

    threadStatus++; // Thread is finished.

    // If both threads are finished, display database table and quit execution.
    if (threadStatus == 2) {
		// SHOW DATABASE
		printf("[SERVER X]: Database Table:\n");
		for(i=0; i<STACK_SIZE; i++){
			printf("[%d]  %d\n", i, s[i]);
		}
		printf("\n");
        printf("[SERVER X]: FINISHED.\n");
        exit(0);
    }
}

// Main function
int main(int argc, char *argv[]) {

    int i = 0;  // Counter variable
    struct info *info = malloc(sizeof(struct info));    // Allocate memory for info struct

    // Generate ticket IDs
    for (i = 0; i < STACK_SIZE; i++) {
        int count;
        int id = (rand() % (32700+1-10000))+10000;  // Create random 5 digit id

        // Check for uniqueness by running id against ids in stack
        if (i > 0) {
            for(count=0; count<STACK_SIZE; count++){
    			while (id == s[count]) {
                    id = (rand() % (32700+1-10000))+10000;  // Regenerate
                }
    		}
        }
        // Push new id onto stack
        push(s, id, &s_top, STACK_SIZE);
    }

    // Display main messages, generated table, and waiting status message
    printf("[SERVER X]: Ticket database generated.\n");
    printf("[SERVER X]: Database Table:\n");
    for(i=0; i<STACK_SIZE; i++){
        printf("[%d]  %d\n", i, s[i]);
    }
    printf("\n");
    printf("[SERVER X]: Waiting for client connections...\n");

    // Variables to hold connection info
    int sockfd, newsockfd, portno, code;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    // Error messages
    if (argc < 2) {
        fprintf(stderr,"ERROR:, no port specified.\n");
        exit(1);
    }

    // Define socket file descriptor
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Error check
    if (sockfd < 0) {
        error("ERROR: could not open socket.");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));  // Clear variables
    portno = atoi(argv[1]);     // Convert argument to int
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind socket
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR: could not bind.");
    }

    // Start listening...
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    // Loop through all connections, and create a new thread to handle them.
    while (1) {
        // Accept connection
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        // Error check
        if (newsockfd < 0) {
            error("ERROR on accept");
        }
        else {
            threadID++;             // Increment threadID
            pthread_t threadID;     // Define new thread
            info->empty = 0;        // Set empty flag for tickets
            info->sock = newsockfd; // Define socket

            // Create new thread to call worker function
            code = pthread_create(&threadID, NULL, thread_worker, info);

            // Error check for creating the thread.
            if (code != 0) {
                close(newsockfd);
                printf("ERROR: cannot create thread :[%s]", strerror(code));
            }
        }
    }

    // Close connection, just in case we get here.
    close(sockfd);
    return 0;
}
