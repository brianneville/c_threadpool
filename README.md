# C threadpool
An implementation of a threadpool, created with C and Pthread

### Details for use:
To see debug information from the c_threadpool.h header file, or the xor_LL.h header file, change the debug macros in their respective files: 

**note: currently both of these are set to 0, and debug information will not be shown.**
**the library must be rebuilt if you wish to see any debug information when using it.**
```
#define DEBUG_XOR_LL 0
#define DEBUG_C_THREADPOOL 0
```

The threadpool is a pool of threads to push functions into. 
The queue is a queue made using my own XOR linked list. It is used to buffer functions before they enter the pool.

See the example_threadpool.c file for a typical example of how to use the threadpool.
Standard protocol would be:
```
 1. initialise a threadpool
 2. prepare threadpool before pushing anything
 3. push functions
 4. repeat steps 2 and 3 if threadpool is not set to exit
 5. cleanup threadpool
```

### Instructions 
Compile example with:
```
cc -Wall example_threadpool.c -o run -L. -ltheadpool -lpthread
```
or see [**compilation instructions**](https://github.com/brianneville/c_threadpool/blob/master/compilation.md) for more details 

and run using:
```
./run
```
