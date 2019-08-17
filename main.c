#define DEBUG_XOR_LL 0

#include <stdlib.h>
#include <stdio.h>
#include "c_threadpool.h"


void test_func(void* args){
	printf("modifying within passed function\n");
	int* arg = (int*)args;
	*arg = 30;
}


int main(){

	Pool threadpool;
	threadpool.pool_size = 3;
	init_pool(&threadpool);

	int args = 3;
	printf("input to queue is %p with contents %d\n",&args, args);
	push_to_queue(&threadpool, test_func, (void*)(&args));
	
	Args* new_args = (Args*)malloc(sizeof(Args));		//struct for args
	new_args->thread_id = (int)7;
	printf("created thread id = %d\n", 7);
	new_args->parent_pool = &threadpool;	
	
	pull_from_queue((void *)(new_args));

	printf("output is %p with contents %d\n",&args, args);
	

	
	return 0;
}