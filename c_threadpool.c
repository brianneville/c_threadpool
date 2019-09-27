#include "c_threadpool.h"

void* pull_from_queue(void* arg){
	Args* args = (Args*)arg;
	int thread_id = args->thread_id;
	Pool* pool = args->pool;
	char *act = (pool->thread_active + thread_id);
	//pop node from queue
	xNode* queue_item;
	while (1){	

		pthread_mutex_lock(&(pool->queue_guard_mtx));
		while (!pool->queue.tail && !(*act)){
			if(DEBUG_C_THREADPOOL)printf("thread %d is sleeping\n",thread_id);
				if (!pool->remaining_work && !pool->queue.tail){
					//if all work from other threads has been done
					pthread_cond_signal(&(pool->block_main));
				}
			pthread_cond_wait(&pool->hold_threads, &(pool->queue_guard_mtx));

		}
		
		queue_item = pop_node_queue(&(pool->queue));
		pthread_mutex_unlock(&(pool->queue_guard_mtx));

		if(queue_item){
			QueueObj* q_obj;
			q_obj = (QueueObj*)queue_item->data;
			function_ptr f = q_obj->func;
			void* args = q_obj->args;
			if(DEBUG_C_THREADPOOL)printf("thread %d is working\n",thread_id);
			(*f)(args);
			if(DEBUG_C_THREADPOOL)printf("thread %d has finished working\n",thread_id);
			free(q_obj);
			pool->remaining_work--;
			*act = 0;
		}
		//break if signalled to terminate on empty queue and queue is empty			
		if (pool->exit_on_empty_queue && !pool->queue.tail && !pool->updating_queue)break;

	}
	pool->living_threads--;
	if(DEBUG_C_THREADPOOL)printf("\033[1;31mExiting thread %d \033[0m\n",thread_id  );
	pthread_exit(NULL);
	

}

void push_to_queue(Pool* pool, function_ptr f, void* args, char block){
	//this function is will be used to store a pointer to a function in a queue
	//pointers must point to function of type void, which takes in a void*
	
	pthread_mutex_lock(&(pool->queue_guard_mtx));		// lock mutex to access queue

	pool->remaining_work++;
	
	QueueObj* insert = (QueueObj*)malloc(sizeof(QueueObj));
	insert->func =f;
	insert->args = args;  //this function can be called with:(*insert->func)(insert->args);
	add_node((void*)insert ,&(pool->queue));		//push into pool queue 
	int i;
	for (i = 0; i < pool->pool_size; i++){
		if(*(pool->thread_active + i)){
			*(pool->thread_active + i) = 1;		//only wake one thread at a time
			break;
		}
	}
	pthread_mutex_unlock(&(pool->queue_guard_mtx));
	if(DEBUG_C_THREADPOOL)printf("\033[1;32mfinished pushing\033[0m\n");

	if(block){
		if(DEBUG_C_THREADPOOL)printf("\nblocking main\n");
		pthread_cond_broadcast(&(pool->hold_threads));
		while(pool->remaining_work ){
			pthread_cond_wait(&(pool->block_main), &(pool->mtx_spare));
		}
		
		pthread_mutex_unlock(&(pool->mtx_spare));
		if(DEBUG_C_THREADPOOL)printf("unblocking main\n\n");
		
		pool->updating_queue = 0;
		pool->living_threads = pool->pool_size;
		
		//now signal all threads which are not active to advance them past the cond_wait() block
		if(pool->exit_on_empty_queue){
			//wait for threads to exit if needed
			memset(pool->thread_active, 1, pool->pool_size*sizeof(char));	//mark all threads as active to escape while loop
			while(pool->living_threads)pthread_cond_broadcast(&(pool->hold_threads));
		}
				
	}
}

void prepare_push(Pool* pool, char exit_on_empty_queue){
	pool->exit_on_empty_queue = exit_on_empty_queue;
	pool->updating_queue = 1;
}

void init_pool(Pool* pool,  char pool_size){	
	// this function initialises a threadpool pointer;
	pool->pool_size = pool_size;
	pool->remaining_work = 0;
	//initialise the linkedlist queue used to push arguemnts into the pool
	init_xLinkedList(&(pool->queue));
	
	//initialse bool to mark whether or not the threads should terminate after the queue is empty
	pool->updating_queue = 0;
	
	//initialise the mutex that the main thread will use to guard items being pulled from this queue
	pthread_mutex_init(&(pool->queue_guard_mtx), NULL);
	
	//initialise variables for blocking main
	pthread_mutex_init(&(pool->mtx_spare), NULL);
	pthread_cond_init(&(pool->block_main), NULL);
	pthread_cond_init(&(pool->hold_threads), NULL);
	//initialise threads and cond variable
	pool->threads_pointer = (pthread_t*)malloc(pool->pool_size * (sizeof(pthread_t)));
	// init all threads to be marked as not running
	pool->thread_active = (char*)calloc(pool->pool_size, pool->pool_size * sizeof(char));	
	
	pthread_t* thr_p = pool->threads_pointer;
	//create pthread with pointer to pull function	
	//initialise cond variables for all threads
	int i;
	for(i=0; i < pool->pool_size; i++){		
		Args* new_args = (Args*)malloc(sizeof(Args));		//struct for args
		new_args->thread_id = i;
		new_args->pool = pool;
		if(DEBUG_C_THREADPOOL)printf("created thread id = %d\n", i);

		pthread_create(thr_p, NULL, pull_from_queue, (void *)(new_args));	// create threads

		while((pool->hold_threads).__align == 2*i);		
		// this ensures that threads are created in order and each thread is created properly
		//(pool->hold_threads).__align is incremented by 2 for every thread created.
		
		//advance pointer
		thr_p += sizeof(pthread_t);
	}
		
}

void cleanup(Pool* pool){
	//free() up memory that has been malloced
	free(pool->threads_pointer);
	free(pool->thread_active);
}


/*
void join_pool(Pool* pool){
	//used for testing
	pthread_t* thr_p = pool->threads_pointer;
	int i;
	for(i=0; i < pool->pool_size; i++){		
		pthread_join((*thr_p), NULL);
		thr_p += sizeof(pthread_t);
	}
}*/