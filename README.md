# Cache Simulator

This is a C program that simulates the operation of cache memory, and it will keep track of hits, misses, and evictions. This cache simulator takes a valgrind memory trace as input, 
simulates the hit/miss behavior of a cache memory on this trace, and outputs the total number of hits, misses, and evictions.

It uses the **FIFO (First In First Out)** replacement policy when choosing which cache line to evict, and takes the following command-line arguments:   

Usage: ./csim -s \<s> -E \<E> -b \<b> -t \<tracefile>
- -s \<s>: Number of set index bits (S = 2 s is the number of sets)
- -E \<E>: Associativity (number of lines per set)
- -b \<b>: Number of block bits (B = 2 b is the block size)
- -t \<tracefile>: Name of the valgrind trace to replay

## Reference Trace Files
The traces subdirectory contains a collection of reference trace files that can be used to evaluate the correctness of the cache simulator. The trace files are generated by a Linux program called valgrind. For example, typing 

`linux> valgrind --log-fd=1 --tool=lackey -v --trace-mem=yes ls -l` 

on the command line runs the executable program `ls -l`, captures a trace of each of its memory accesses in the order they occur, and prints them on `stdout`. Valgrind memory traces have the following form: 

I 0400d7d4,8  
M 0421c7f0,4  
L 04f6b868,8  
S 7ff0005c8,8 

Each line denotes one or two memory accesses. The format of each line is  

**[space]operation address,size**

The operation field denotes the type of memory access: “I” denotes an instruction load, “L” a data load, “S” a data store, and “M” a data modify (i.e., a data load followed by a data store). There is never a space before each “I”. There is always a space before each “M”, “L”, and “S”. The address field specifies a 64-b hexadecimal memory address. The size field specifies the number of bytes accessed by the operation.
