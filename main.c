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
	push_to_queue(threadpool, test_func, (void*)(&args), 1);
	
	prepare_push(threadpool, 1);

	push_to_queue(threadpool, test_func, (void*)(&args), 1);
	
	printf("args is now %d\n", args);
	/*
	prepare_push(threadpool, 1);	//exit_on_empty_queue = 1
	push_to_queue(threadpool, test_func, (void*)(&args), 0);
	//push_to_queue(threadpool, test_func, (void*)(&args), 0);
	push_to_queue(threadpool, test_func, (void*)(&args), 1);
	
	printf("args is now %d\n", args);
	*/
		
	//join_pool(threadpool);		// join threads after finishing
	
	
	return 0;
}