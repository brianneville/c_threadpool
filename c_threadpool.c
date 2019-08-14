#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "c_threadpool.h"

void * pull_from_queue(void* arg){
	printf("Thread has started pulling from queue\n");
	printf("arg is %lu\n", (unsigned long)arg);

}

void push_to_queue(){
		int i = 0;
	
}


void init_pool(Pool* pool_inst){
	// initialise a threadpool pointer;
	pool_inst->threads_pointer = (pthread_t*)malloc(pool_inst->pool_size * (sizeof(pthread_t)));
	pool_inst->cond_pointer = (pthread_cond_t*)malloc(pool_inst->pool_size * (sizeof(pthread_cond_t)));
	
	pthread_cond_t* cond_p = pool_inst->cond_pointer;
	pthread_t* thr_p = pool_inst->threads_pointer;
	//create pthread with pointer to empty function	
	//initialise mutex variables for all threads
	unsigned long i;
	for(i=0; i < pool_inst->pool_size; i++){		
		pthread_cond_init(cond_p, NULL);
		printf("creating thread\n");
		int x = pthread_create(thr_p, NULL, pull_from_queue, (void *)(i));
		
		cond_p += sizeof(pthread_cond_t);
		thr_p += sizeof(pthread_t);
	}
	thr_p -= pool_inst->pool_size* sizeof(pthread_t);
	for(i=0; i < pool_inst->pool_size; i++){		
		pthread_join((*thr_p), NULL);
		thr_p += sizeof(pthread_t);
	}
	
}