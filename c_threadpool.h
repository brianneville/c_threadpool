#ifndef C_THREADPOOL_H_
#define C_THREADPOOL_H_
#include <pthread.h>


typedef struct Pool{
	int pool_size;
	pthread_t* threads_pointer;	
	pthread_cond_t* cond_pointer;
}Pool;


//void *pull_from_queue(void* arg);
void *pull_from_queue(void* arg);
void push_to_queue();


#endif