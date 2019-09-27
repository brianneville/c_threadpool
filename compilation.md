# Different ways to build the binary:

#### 1. build from .c files
```
cc -Wall example_threadpool.c xor_LL.c c_threadpool.c -o run -lpthread
./run
```

#### 2. build from objects
```
cc -c c_threadpool.c
cc -c xor_LL.c
cc -Wall example_threadpool.c xor_LL.o c_threadpool.o -o run -lpthread
rm xor_LL.o 
rm c_threadpool.o
./run 
```

#### 3. create a library, and link this library while compiling
```
cc -c c_threadpool.c
cc -c xor_LL.c
ar rcs libthreadpool.a xor_LL.o c_threadpool.o 
cc -Wall example_threadpool.c -o run -L. -lthreadpool -lpthread
rm c_threadpool.o
rm xor_LL.o
./run
```
