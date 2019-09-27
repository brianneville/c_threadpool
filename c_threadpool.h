#ifndef C_THREADPOOL_H
#define C_THREADPOOL_H

#ifndef DEBUG_C_THREADPOOL
#define DEBUG_C_THREADPOOL 0		// change this to view debug information on the thread pool
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "c_threadpool.h"
#include "xor_LL.h"

typedef struct Pool{
	int pool_size;
	pthread_t* threads_pointer;		// used to iterate through threads
	char* thread_active;			// booleans marked as true if the thread is running
	
	xLinkedList queue; 				// queue used to push items into the pool
	pthread_mutex_t queue_guard_mtx; // mtx to guard queue for consumers
	
	char exit_on_empty_queue; 	//if true, then the threads will terminate after the queue has been filled and then emptied
	char updating_queue; 		//set this to 1 before using the push_to_queue() function, and set it to 0 afterwards
								//this is used to tell the threads pulling from queue that other objects are going to be		
								//inserted into the queue.
								//(prevents issue where thread could finish work before next function has been added to queue,
								// while exit_on_empty_queue is set to 1. this would cause premature exit).	
	
	long remaining_work;		//used to check if the queue is empty. empty queue implies all functions are working or finished
	int living_threads;			//used to track if all threads have exited
	pthread_cond_t block_main; 		//used to block main thread while other threads execute
	pthread_mutex_t mtx_spare;		//spare, useless mutex for the condition variable (semaphores had reliability issues)

	pthread_cond_t hold_threads;	//this condition variable makes threads sleep when finished working in an empty queue
	
}Pool;


typedef struct Args{
	int thread_id;
	Pool* pool;	
}Args;


typedef void (*function_ptr)(void* args);
typedef struct QueueObj{
	function_ptr func;
	void* args;
}QueueObj;

//function calls for the user
void init_pool(Pool* pool,  char pool_size);								// initialise a new threadpool
void prepare_push(Pool* pool, char exit_on_empty_queue);					//call this to prepare the threadpool for pushing
void push_to_queue(Pool* pool, function_ptr f, void* args, char block);	//push function to queue for pool. 
																				//set block == 1 to block in main thread
void cleanup(Pool* pool);													//release all malloced memory back to the heap
//void join_pool(Pool* pool);

void* pull_from_queue(void* arg);

#endif