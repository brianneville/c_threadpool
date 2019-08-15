#ifndef C_THREADPOOL_H
#define C_THREADPOOL_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "c_threadpool.h"
#include <pthread.h>
#include "xor_LL.h"

typedef struct Pool{
	int pool_size;
	pthread_t* threads_pointer;		// used to iterate through threads
	pthread_cond_t* cond_pointer;	// used to iterate through corresponding cond variables
	char* thread_active;			// booleans marked as true if the thread is running
	
	xLinkedList queue; 				// queue used to push items into the pool
	pthread_mutex_t queue_guard_mtx;

}Pool;


typedef struct Args{
	int thread_id;
	Pool* parent_pool;	
}Args;

/*
typedef struct FuncArgs{
	void* input_args;
	void* output_args;	
}FuncArgs;
*/

typedef void (*function_ptr)(void* args);
typedef struct QueueObj{
	function_ptr func;
}QueueObj;



void init_pool(Pool* pool_inst);
void* pull_from_queue(void* arg);
void push_to_queue( function_ptr f, void* args);

void* pull_from_queue(void* arg){
	Args* args = (Args*)arg;
	printf("Thread has started pulling from queue, with id = %d\n", args->thread_id);
}

void push_to_queue(function_ptr f, void* args){
	//this function is will be used to store a pointer to a function in a queue
	//pointers must point to function of type void, which takes in a void*
	QueueObj* insert = (QueueObj*)malloc(sizeof(QueueObj));
	insert->func =f;
	// call this function with: (*insert->func)();
	(*insert->func)(args);
}

void init_pool(Pool* pool_inst){
	// this function initialises a threadpool pointer;
	
	//initialise the linkedlist queue used to push arguemnts into the pool
	pool_inst->queue.tail = NULL;
	
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
		new_args->parent_pool = pool_inst;
		
		int x = pthread_create(thr_p, NULL, pull_from_queue, (void *)(new_args));	// create
		
		//advance pointers
		cond_p += sizeof(pthread_cond_t);
		thr_p += sizeof(pthread_t);
	}
	
	//join threads ( for testing purposes)
	thr_p -= pool_inst->pool_size* sizeof(pthread_t);
	for(i=0; i < pool_inst->pool_size; i++){		
		pthread_join((*thr_p), NULL);
		thr_p += sizeof(pthread_t);
	}

}

#endif