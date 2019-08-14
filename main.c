#include <stdlib.h>
#include <stdio.h>
#include "c_threadpool.h"


int main(){
	Pool threadpool;
	threadpool.pool_size = 3;
	init_pool(&threadpool);
	
	return 0;
}