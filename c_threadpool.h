#ifndef C_THREADPOOL_H
#define C_THREADPOOL_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "c_threadpool.h"
#include "xor_LL.h"

typedef struct Pool{
	int pool_size;
	pthread_t* threads_pointer;		// used to iterate through threads
	pthread_cond_t* cond_pointer;	// used to iterate through corresponding cond variables
	char* thread_active;			// booleans marked as true if the thread is running
	
	xLinkedList queue; 				// queue used to push items into the pool
	pthread_mutex_t queue_guard_mtx; // mtx to guard queue for consumers
	
	char exit_on_empty_queue; 	//if true, then the threads will terminate after the queue has been filled and then emptied
	char updating_queue; 		//set this to 1 before using the push_to_queue() function, and set it to 0 afterwards
								//this is used to tell the threads pulling from queue that other objects are going to be		
								//inserted into the queue.
								//(prevents issue where thread could finish work before next function has been added to queue,
								// while exit_on_empty_queue is set to 1. this would cause premature exit).	
	long remaining_work;
	int living_threads;
}Pool;


typedef struct Args{
	int thread_id;
	Pool* pool_inst;	
}Args;


typedef void (*function_ptr)(void* args);
typedef struct QueueObj{
	function_ptr func;
	void* args;
}QueueObj;

void init_pool(Pool* pool_inst,  char pool_size);
void close_pool(Pool* pool_inst);
void* pull_from_queue(void* arg);
void push_to_queue(Pool* pool_inst, function_ptr f, void* args, char block);

void* pull_from_queue(void* arg){
	Args* args = (Args*)arg;
	int thread_id = args->thread_id;
	printf("Thread has started pulling from queue, with id = %d\n", thread_id);
	Pool* pool_inst = args->pool_inst;
	
	//printf("head of queue is %p\n",args->pool_inst->queue.head->data );

	//pop node from queue
	xNode* queue_item;
	while (1){	
		pthread_mutex_lock(&(pool_inst->queue_guard_mtx));	

		if (!pool_inst->queue.tail && !(*(pool_inst->thread_active + thread_id))){
			//queue is empty, go to sleep until woken by the corresponding condition variable
			printf("thread %d is sleeping on condition var %p\n",thread_id, pool_inst->cond_pointer + thread_id * sizeof(pthread_cond_t) );

			pthread_cond_wait((pool_inst->cond_pointer + thread_id * sizeof(pthread_cond_t)),
				&(pool_inst->queue_guard_mtx));
		}
		char *act = (pool_inst->thread_active + thread_id);
		*act = 1;
		queue_item = pop_node_queue(&(pool_inst->queue));
		printf("thread %d has been woken, ", thread_id);
		if (queue_item)printf("thread has recieved queue item:\n");
		
		
		pthread_mutex_unlock(&(pool_inst->queue_guard_mtx));

		if(queue_item){
			QueueObj* q_obj;
			q_obj = (QueueObj*)queue_item->data;
			function_ptr f = q_obj->func;
			void* args = q_obj->args;
			(*f)(args);
			pool_inst->remaining_work--;
		}
		*act = 0;
		//break if signalled to terminate on empty queue and queue is empty
		if (pool_inst->exit_on_empty_queue && !pool_inst->queue.tail && !pool_inst->updating_queue)break;
	}
	pool_inst->living_threads--;
	printf("exiting thread %d\n",thread_id  );
	pthread_exit(NULL);

}

void push_to_queue(Pool* pool_inst, function_ptr f, void* args, char block){
	//this function is will be used to store a pointer to a function in a queue
	//pointers must point to function of type void, which takes in a void*
	
	pthread_mutex_lock(&(pool_inst->queue_guard_mtx));		// lock mutex to access queue

	pool_inst->remaining_work++;
	
	QueueObj* insert = (QueueObj*)malloc(sizeof(QueueObj));
	insert->func =f;
	insert->args = args;  //this function can be called with:(*insert->func)(insert->args);
	add_node((void*)insert ,&(pool_inst->queue));		//push into pool queue 
	
	pthread_mutex_unlock(&(pool_inst->queue_guard_mtx));

	//signal a condition variable which is not working
	int i;
	for(i =0; i < pool_inst->pool_size; i ++){
		if(!(*(pool_inst->thread_active + i))){
			//if thread is not active then wake it to pull from queue
				pthread_cond_signal(pool_inst->cond_pointer + i* sizeof(pthread_cond_t));
				break;
		}
	}
	printf("finished pushing\n");

	if(block){
		printf("\nblocking main\n");
		while(pool_inst->remaining_work);
		printf("\nunblocking main\n");

		pool_inst->updating_queue = 0;
		if(pool_inst->exit_on_empty_queue){
			
			pool_inst->living_threads = pool_inst->pool_size;
			//now signal all threads which are not active to advance them past the cond_wait() block
			for(i =0; i < pool_inst->pool_size; i ++){
				if(!(*(pool_inst->thread_active + i))){
					pthread_cond_signal(pool_inst->cond_pointer + i* sizeof(pthread_cond_t));
				}
			}
			while(pool_inst->living_threads);
		}
	}
}

void prepare_push(Pool* pool_inst, char exit_on_empty_queue){
	pool_inst->exit_on_empty_queue = exit_on_empty_queue;
	pool_inst->updating_queue = 1;
}

void join_pool(Pool* pool_inst){
	//launch threads to close all current threads in the pool in parallel
	
	
	pthread_t* thr_p = pool_inst->threads_pointer;
	int i;
	for(i=0; i < pool_inst->pool_size; i++){		
		pthread_join((*thr_p), NULL);
		thr_p += sizeof(pthread_t);
	}
	
}

void init_pool(Pool* pool_inst,  char pool_size){
	// this function initialises a threadpool pointer;
	pool_inst->pool_size = pool_size;
	pool_inst->remaining_work = 0;
	//pool_inst->exited_thread_count = 0;
	//initialise the linkedlist queue used to push arguemnts into the pool
	init_xLinkedList(&(pool_inst->queue));
	
	//initialse bool to mark whether or not the threads should terminate after the queue is empty
	//pool_inst->exit_on_empty_queue = NULL;
	pool_inst->updating_queue = 0;
	//initialise the mutex that the main thread will use to guard items being pulled from this queue
	pthread_mutex_init(&(pool_inst->queue_guard_mtx), NULL);
	
	pool_inst->threads_pointer = (pthread_t*)malloc(pool_inst->pool_size * (sizeof(pthread_t)));
	pool_inst->cond_pointer = (pthread_cond_t*)malloc(pool_inst->pool_size * (sizeof(pthread_cond_t)));
	// init all threads to be marked as not running
	pool_inst->thread_active = (char*)calloc(pool_inst->pool_size, pool_inst->pool_size * sizeof(char));	
	
	pthread_cond_t* cond_p = pool_inst->cond_pointer;
	pthread_t* thr_p = pool_inst->threads_pointer;
	//create pthread with pointer to empty function	
	//initialise cond variables for all threads
	unsigned long i;
	for(i=0; i < pool_inst->pool_size; i++){		
		pthread_cond_init(cond_p, NULL);		// init cond variable
		
		Args* new_args = (Args*)malloc(sizeof(Args));		//struct for args
		new_args->thread_id = (int)i;
		printf("created thread id = %lu\n", i);
		new_args->pool_inst = pool_inst;
		
		int x = pthread_create(thr_p, NULL, pull_from_queue, (void *)(new_args));	// create
		//advance pointers
		cond_p += sizeof(pthread_cond_t);
		thr_p += sizeof(pthread_t);
	}
		
}

void cleanup(Pool* pool_inst){
	//free() up memory that has been malloc
}

#endif