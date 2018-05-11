/* Enrique Torres
 * minor5.c - using producer-consumer paradigm.
 * Descripttion: This program is a producer and consumer program that the producer adds data to the buffer and the consumers retrieves the data 
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NITEMS 10		// number of items in shared buffer

// shared variables
char shared_buffer[NITEMS];	// echo buffer
int shared_count;		// item count
//int fill;
//int empty;
pthread_cond_t c = PTHREAD_COND_INITIALIZER; // pthread condition variable for consumer
pthread_cond_t c2 = PTHREAD_COND_INITIALIZER; // pthread condition variable for producer
pthread_mutex_t mutex;		// pthread mutex
unsigned int prod_index = 0; 	// producer index into shared buffer
unsigned int cons_index = 0; 	// consumer index into shard buffer
int worktodo = 0;
// function prototypes
void * producer(void *arg);
void * consumer(void *arg);

int main() 
{ 
	pthread_t prod_tid, cons_tid1, cons_tid2; 
	
	// initialize pthread variables
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&c, NULL); //initialize the consumer condition to null
	pthread_cond_init(&c2, NULL); //initialize the producer condition to null
	// start producer thread
	 pthread_create(&prod_tid, NULL, producer, NULL);

	// start consumer threads
	pthread_create(&cons_tid1, NULL, consumer, NULL); //consumer thread 1
	pthread_create(&cons_tid2, NULL, consumer, NULL); //consumer thread 2
	
	// wait for threads to finish
	pthread_join(prod_tid, NULL);
	pthread_join(cons_tid1, NULL);
	pthread_join(cons_tid2, NULL);
			
	// clean up
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&c);
	pthread_cond_destroy(&c2);
	return 0;
}

// producer thread executes this function
void * producer(void *arg)
{
	char key;
	int i = 0;
	while (1)
	{
		// read input key
		scanf("%c", &key);
		
	       	while(1) //while (shared_count <= NITEMS)
		  {
		  
			// acquire mutex lock
		     pthread_mutex_lock(&mutex); //locks the thread
			  
			// if buffer is full, release mutex lock and check again
			if (shared_count == NITEMS)
			  { 
			    pthread_cond_wait(&c2, &mutex); //waits until the buffer is full
			      
		            pthread_mutex_unlock(&mutex);
			  }
			else
			  
				break;
		  }	

		// store key in shared buffer
		shared_buffer[prod_index] = key;

		// update shared count variable
		shared_count++;
		//pthread_cond_signal(&c); //signals to the consumer
		// update producer index
		if (prod_index == NITEMS - 1)
			prod_index = 0;
		else
			prod_index++;
		
		// release mutex lock
		pthread_cond_signal(&c);
		pthread_mutex_unlock(&mutex);  //unlocks the thread
			}

	return NULL;
	
}

// consumer thread executes this function
void * consumer(void *arg)
{
	char key;

	int id = (int)pthread_self();

	while (1)
	{
	   while (1) //while(shared_count <= NITEMS)
	    {
			// acquire mutex lock
	  pthread_mutex_lock(&mutex); 
			//pthread_cond_signal(&c2);
			// if buffer is empty, release mutex lock and check again
			if (shared_count == 0)
			{
			   pthread_cond_wait(&c, &mutex);
			   //    pthread_cond_signal(&c2);
			   	pthread_mutex_unlock(&mutex);
			}
				else
				 
				break;
				}

		// read key from shared buffer
		key = shared_buffer[cons_index]; //dont change
		
		// echo key
		printf("consumer %d %c\n", id, key);

		// update shared count variable
		shared_count--;
		//pthread_cond_signal(&c2); //signal to the producer
		// update consumer index
		if (cons_index == NITEMS - 1) //dont change
		  cons_index = 0;             //dont change
		else
		  cons_index++;        //dont change
	
		// release mutex lock
		pthread_cond_signal(&c2);
		pthread_mutex_unlock(&mutex);
	}

	return NULL;
}
