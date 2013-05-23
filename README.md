multithread_sort
================

Multithread Sort

Sort an array in multiple threads - used for multi-thread performance testing

Compile: gcc -O2 -o multithread_sort multithread_sort.c -l pthread -lm

Usage: ./multithread_sort N P

where N - 2**N partitions (number of threads)
      P - 2**P integer array size
      

      
      
