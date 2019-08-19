#define DEBUG_XOR_LL 0

#include <stdlib.h>
#include <stdio.h>
#include "c_threadpool.h"


void test_func(void* args){
	printf("\t\tmodifying within passed function\n");
	int* arg = (int*)args;
	*arg = *arg + 1;
	sleep(3);
}


int main(){

	Pool* threadpool = (Pool*)malloc(sizeof(Pool));
	int pool_size = 3;
	init_pool(threadpool, pool_size);	//pool_size = 3, 
	
	int args = 0;
	printf("input to queue is %p with contents %d\n",&args, args);
	prepare_push(threadpool, 0);		//exit_on_empty_queue = 0
	push_to_queue(threadpool, test_func, (void*)(&args), 0);
	push_to_queue(threadpool, test_func, (void*)(&args), 0);
	push_to_queue(threadpool, test_func, (void*)(&args), 0);
	push_to_queue(threadpool, test_func, (void*)(&args), 1);
	
	printf("args is now %d\n", args);
	
	prepare_push(threadpool, 1);	//exit_on_empty_queue = 1
	push_to_queue(threadpool, test_func, (void*)(&args), 0);
	push_to_queue(threadpool, test_func, (void*)(&args), 0);
	push_to_queue(threadpool, test_func, (void*)(&args), 1);
	
	printf("args is now %d\n", args);
	/*
	threadpool->exit_on_empty_queue = 1;
	push_to_queue(threadpool, test_func, (void*)(&args), 0);
	push_to_queue(threadpool, test_func, (void*)(&args), 0);
	push_to_queue(threadpool, test_func, (void*)(&args), 1);
	*/
	//wake a thread
	//printf("signalling %p \n",threadpool->cond_pointer + 2* sizeof(pthread_cond_t) );
	//pthread_cond_signal(threadpool->cond_pointer + 2* sizeof(pthread_cond_t));
	/*
	Args* new_args = (Args*)malloc(sizeof(Args));		//struct for args
	new_args->thread_id = (int)7;
	printf("created thread id = %d\n", 7);
	new_args->pool_inst = threadpool;	
	
	pull_from_queue((void *)(new_args));

	printf("output is %p with contents %d\n",&args, args);
*/	
	//await_pool(threadpool);
		
	//join_pool(threadpool);		// join threads after finishing
	
	/////idea: block while any threads are active
	
	return 0;
}