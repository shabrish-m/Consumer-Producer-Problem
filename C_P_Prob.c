/* 
Program to solve the Consumer Producer Problem 
with Synchronisation and Mutex 
Saira Barlas
08/12/2014
*/

//Creating the type buffer_item
// buffer
typedef int buffer_item;

//Buffer of size 5 aka 5 spaces
#define BUFFER_SIZE 5

/* main.c */

//All the headers needed for the program in C
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h> //Included for pthreads
#include <semaphore.h> //Needed for the semaphores
//#include "buffer.h"

//This is needed to produce the waiting time
#define RAND_DIVISOR 100000000
#define TRUE 1

//Lock which will block when the producers is full and block consumers when the buffer is empty
/* The mutex lock */
pthread_mutex_t mutex;

//The semaphores needed for blocking when the buffer's state is full or when empty
/* the semaphores */
sem_t full, empty;

//Officially creates the buffer and gives it the size of 5 spaces
/* the buffer */
buffer_item buffer[BUFFER_SIZE];

//Each space existent in the buffer needed for iteration purposes
/* buffer counter */
int counter;

//Unique ID of the thread(s) and the default attributes
pthread_t tid;       //Thread ID
pthread_attr_t attr; //Set of thread attributes

//Producer thread which lets pointer param be of any data type
void *producer(void *param); /* the producer thread */

//Consumer thread which lets pointer param be of any data type
void *consumer(void *param); /* the consumer thread */

//Setting up the data for the threads and semaphores
void initializeData() {

//The lock is created and is given a NULL value so it can be terminated properly
   /* Create the mutex lock */
   pthread_mutex_init(&mutex, NULL);

//The full semaphore which is the size of 0 aka full and no spaces and shall act when that condition within a buffer is met
   /* Create the full semaphore and initialize to 0 */
   sem_init(&full, 0, 0);


//The empty semaphore which is the size of the maximum spaces available in a buffer and shall act when that condition has been met within a buffer
   /* Create the empty semaphore and initialize to BUFFER_SIZE */
   sem_init(&empty, 0, BUFFER_SIZE);

//The default attributes of a thread initially
   /* Get the default attributes */
   pthread_attr_init(&attr);

//The counter of the buffer by default is set to 0 and hence can be incremented or decremented using loops
   /* init buffer */
   counter = 0;
}

//The attributes of the Producer thread
/* Producer Thread */
void *producer(void *param) {

//An item that can be added to the buffer
   buffer_item item;

//When the producer thread is being created
   while(TRUE) {

//Causing the main method to sleep before termination
      /* sleep for a random period of time */

//Random number is generated which is then divided by the milliseconds
      int rNum = rand() / RAND_DIVISOR;

//The time generated is then used for sleeping purposes
      sleep(rNum);

//A rand function is used to create a very random number used to fill in the space in the buffer
      /* generate a random number */
      item = rand();

//Use the empty lock when the buffer is empty and hence will wait for Consumer
      /* acquire the empty lock */
      sem_wait(&empty);

//Mutex lock is used to make sure that only one process may execute the section at a time and controls accesses to the buffer
      /* acquire the mutex lock */
      pthread_mutex_lock(&mutex);

//If item being inserted has no space, print an error
      if(insert_item(item)) {
         fprintf(stderr, " Producer report error condition\n");
     }

//Else allocate a space to the item and print it     
      else {
         printf("producer produced %d\n", item);
      }

//Allow for other process to start
      /* release the mutex lock */
      pthread_mutex_unlock(&mutex);

//Send a signal to the Consumer letting it know there are items in the buffer
      /* signal full */
      sem_post(&full);
   }
}

//The attributes of the Consumer thread
/* Consumer Thread */
void *consumer(void *param) {

//An item that can be added to the buffer
   buffer_item item;

//When the Consumer thread is being created
   while(TRUE) {

//Causing the main method to sleep before termination
      /* sleep for a random period of time */

//Random number is generated which is then divided by the milliseconds
      int rNum = rand() / RAND_DIVISOR;

//The time generated is then used for sleeping purposes
      sleep(rNum);

//Use the full lock when the buffer is full and hence will wait for Producer
      /* acquire the full lock */
      sem_wait(&full);

//Mutex lock is used to make sure that only one process may execute the section at a time and controls accesses to the buffer
      /* aquire the mutex lock */
      pthread_mutex_lock(&mutex);

//If no more items being removed exist, print an error
      if(remove_item(&item)) {
         fprintf(stderr, "Consumer report error condition\n");
      }

//Else remove an item from the buffer and print out
      else {
         printf("consumer consumed %d\n", item);
      }

//Allow for other process to start
      /* release the mutex lock */
      pthread_mutex_unlock(&mutex);

//Send a signal to the Producer letting it know there are spaces in the buffer
      /* signal empty */
      sem_post(&empty);
   }
}

//How an item is inserted into the buffer
/* Add an item to the buffer */
int insert_item(buffer_item item) {

//If there is space then add the item and fill up the spaces in the buffer
   /* When the buffer is not full add the item
      and increment the counter */
   if(counter < BUFFER_SIZE) {
      buffer[counter] = item;
      counter++;
      return 0;
   }

//If the buffer is full, return an error ie. "something went wrong"
   else { /* Error the buffer is full */
      return -1;
   }
}

//How an item is removed from the buffer
/* Remove an item from the buffer */
int remove_item(buffer_item *item) {

//If there are filled spaces, take them out of the buffer and free up a space
   /* When the buffer is not empty remove the item
      and decrement the counter */
   if(counter > 0) {
      *item = buffer[(counter-1)];
      counter--;
      return 0;
   }

//If the whole buffer is empty, return an error ie. "something went wrong"
   else { /* Error buffer empty */
      return -1;
   }
}

//Creating and displaying the threads and arguments
int main(int argc, char *argv[]) {
   /* Loop counter */
   int i;

//If there are only 3 arguments then do this
   /* Verify the correct number of arguments were passed in */
   if(argc != 4) {
      fprintf(stderr, "USAGE:./main.out <INT> <INT> <INT>\n");
   }

//The time the program has until it sleeps and terminates
   int mainSleepTime = atoi(argv[1]); /* Time in seconds for main to sleep */

//The number of threads that shall be produced for the Producer  
   int numProd = atoi(argv[2]); /* Number of producer threads */

//The number of threads that shall be produced for the Consumer
   int numCons = atoi(argv[3]); /* Number of consumer threads */

//All data shall be used now
   /* Initialize the app */
   initializeData();

//Create the number of Producer threads with the ID, default attributes and is NULL
   /* Create the producer threads */
   for(i = 0; i < numProd; i++) {
      /* Create the thread */
      pthread_create(&tid,&attr,producer,NULL);
    }

//Create the number of Consumer threads with ID, default attributes and is NULL
   /* Create the consumer threads */
   for(i = 0; i < numCons; i++) {
      /* Create the thread */
      pthread_create(&tid,&attr,consumer,NULL);
   }

//Then afterwards sleep
   /* Sleep for the specified amount of time in milliseconds */
   sleep(mainSleepTime);

//And exit the program ie. terminate
   /* Exit the program */
   printf("Exit the program\n");
   exit(0);
}

