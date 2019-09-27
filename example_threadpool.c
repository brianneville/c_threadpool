#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "c_threadpool.h"


void test_func(void* args){
	printf("\t\tmodifying within passed function\n");
	int* arg = (int*)args;
	*arg = *arg + 1;
	//simulate work being done
	sleep(1+rand()%3);
	
}


int main(){
	srand(999);
	
	Pool* threadpool = (Pool*)malloc(sizeof(Pool));
	init_pool(threadpool, 3);	//pool_size = 3 
	
	int args = 0;
	printf("input to queue is %p with contents %d\n",&args, args);
	
	prepare_push(threadpool, 0);	//exit_on_empty_queue = 0 to reuse threadpool
	push_to_queue(threadpool, test_func, (void*)(&args), 0);
	push_to_queue(threadpool, test_func, (void*)(&args), 0);
	push_to_queue(threadpool, test_func, (void*)(&args), 0);
	push_to_queue(threadpool, test_func, (void*)(&args), 1);	//block = 1 on final push call
	
	printf("args is now %d\n", args);
	
	prepare_push(threadpool, 0);	//exit_on_empty_queue = 0
	push_to_queue(threadpool, test_func, (void*)(&args), 0);
	push_to_queue(threadpool, test_func, (void*)(&args), 0);
	push_to_queue(threadpool, test_func, (void*)(&args), 1);
	
	printf("args is now %d\n", args);
	
	prepare_push(threadpool, 1);	//exit_on_empty_queue = 1
	push_to_queue(threadpool, test_func, (void*)(&args), 1);
	
	printf("args is now %d\n", args);
	
	cleanup(threadpool);	
	
	
	return 0;
}
