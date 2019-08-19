# C threadpool
An implementation of a threadpool, using the Pthread library

### Details for use:
To see debug information from the c_threadpool.h header file, or the xor_LL.h header file, change the following statements:
```
#define DEBUG_XOR_LL 0
#define DEBUG_C_THREADPOOL 1
```

The threadpool is a pool of threads to push functions into. 
The queue is a queue made using my own XOR linked list. it is used to buffer functions before they enter the pool

See the example_threadpool.c file for a typical example of how to use the threadpool.
Standard protocol would be:
```
 1. initialise a threadpool
 2. prepare threadpool before pushing anything
 3. push functions
 4. repeat if threadpool is not set to exit
 5. cleanup threadpool
```

### Instructions 
Compile example with:
```
cc example_threadpool.c -o run -lpthread
```
and run using:
```
./run
```
