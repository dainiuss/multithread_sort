/**
 * Program: multithread_sort.c
 * Compile: gcc -O2 -o multithread_sort multithread_sort.c -l pthread -lm
 * Author:  Dainius
 * Date:    2013-05-10
 */
 
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int *minArray;
int *randomArray;
int **arrayOfArrays;
int **arrayOfMinArrays;

/**
 * thread_info struct - holds thread info
 * code reference - linux man pages: man pthread_create
 */
struct thread_info {  
   pthread_t tid; 
   int threadIndex;
   int localThreadIndex;
   int startIndex;
   int endIndex;
   int arrayNumber;
};

/**
 * Display values in array - used for debugging
 */
void showArray(int a[], int size){
    int i;
    printf("\nARRAY: [");
    for(i=0; i<size; i++){
        if(i==size-1){
            printf("%d", a[i]);
        }
        else{
            printf("%d ", a[i]);
        }
    }
    printf("]\n");
}

/**
 * Copy array
 */
void copyArray(int a[], int size, int *copy){
    int i;
    for(i=0; i<size; i++){
        copy[i] = a[i];
    }
}

/**
 * Display values in array - used for debugging
 */
void showDoubleArray(double a[], int size){
    int i;
    printf("\nARRAY: [");
    for(i=0; i<size; i++){
        if(i==size-1){
            printf("%f", a[i]);
        }
        else{
            printf("%f ", a[i]);
        }
    }
    printf("]\n");
}

/**
 * Insertion sort - sorts the array
 * @param a[] - array to sort
 * @param size - size of array
 */
void insertionSort(int a[], int start, int end){
    int i;
    int j;
    int temp;
    int size = end;
    
    for(i=start; i<size; i++){
        /*  Put value to temp */
        temp = a[i];
        for(j=i-1; j>=0; j--){
            /* Shift array values until we find < temp then break */
            if(a[j] <= temp){
                break;
            }
            a[j+1] = a[j];
        }
        /* Put temp to it's new location */
        a[j+1] = temp;
    }
}

/**
 * threadSort function
 */
void threadSort(void * arguments) {
    struct thread_info *thread = (struct thread_info *) arguments;
    int globalindex = thread->threadIndex;
    int localindex = thread->localThreadIndex;
    int start = thread->startIndex;
    int end   = thread->endIndex;
    int arrayNumber = thread->arrayNumber;
 
    /* Sort this thread's partition for arrayNumber array */
    insertionSort(arrayOfArrays[arrayNumber], start, end);
    
    /* Add this thread minimum to the global min array */
    arrayOfMinArrays[arrayNumber][localindex] = start;
}

/**
 * get_next()
 */
int get_next(int numberOfPartitions, int partitionSize, int a[], int minArray[]) {
    int i;
    int partitionEnd;
    int smallest = minArray[0];
    int minIndex = 0;
    int current = 0;
    
    for(i=1; i<numberOfPartitions; i++) {
        current = a[minArray[i]];
        partitionEnd = ((i+1)*partitionSize) - 1;
        
        if(current < smallest && minArray[i] < partitionEnd) {
            minIndex = i;
            smallest = current;
        }
    }
    
    minArray[minIndex]++;
    return smallest;
}

/**
 * MAIN FUNCTION
 */
int main( int argc, char *argv[] ) {
    int i, j, retvalue, counter, arrayNumber, globalThreadIndex;
    int numberOfMinArrays;
    int n;
    int p;
    int numberOfPartitions;
    int numberOfThreads;
    int numberOfArrays;
    int sizeOfArray;
    int partitionSize;
    int globalIndex;
    double generateTime;
    int *sorted;
    
    struct timeval time1;
    struct timeval time2;
    struct timeval time3;
    struct timeval time4;

    if(argc != 3){
        printf("NUMBERS OF ARGUMENTS PASSED: %d\n", argc-1);
        printf("MUST PASS 2 ARGUMENTS\n");
  	printf("Usage: %s N P\n", argv[0]);
		printf("ARGUMENTS:\n");
		printf("N     - 2**N partitions\n");
		printf("P     - 2**P integer array of size\n");
		exit(-1);
	}
    fprintf(stderr,"------------------------------------------------------------------------------\n");
	
	/* Getting command line arguments */
	n = atoi(argv[1]);
    p = atoi(argv[2]);
    
    /* Get array size and number of partitions */
    numberOfPartitions = pow(2, n);
    sizeOfArray = pow(2, p);
    
    /* Number of threads = number of partitions */
    numberOfThreads = numberOfPartitions;
    
    /* Partition size 2^(p-n) */
    partitionSize = sizeOfArray / numberOfPartitions;
    
    /* Number of arrays = partition size + 1 */
    numberOfArrays = n + 1;
    
    /* Number of minArrays = numberOfArrays */
    numberOfMinArrays = numberOfArrays ;
    
    /* Allocate memory for minArray */
    minArray = malloc(sizeof(int) * numberOfPartitions);
    
    /* Allocate memory for randomArray */
    randomArray = malloc(sizeof(int) * sizeOfArray);
    
    /* Allocate memory for sorted array */
    sorted = malloc(sizeof(int) * sizeOfArray);
    
    /* Allocate memory for arrayOfArrays */
    arrayOfArrays = (int**)malloc(numberOfArrays * sizeof(int*));
    for(i=0; i<numberOfArrays; i++){
        arrayOfArrays[i] = (int*)malloc(sizeOfArray * sizeof(int));
    }
    
    counter = numberOfPartitions;
    /* Allocate memory for arrayOfMinArrays */
    arrayOfMinArrays = (int**)malloc(numberOfMinArrays * sizeof(int*));
    for(i=0; i<numberOfMinArrays; i++){
        arrayOfMinArrays[i] = (int*)malloc(counter * sizeof(int));
        /* Size of first array - 8, second - 4, third - 2, etc. */
        counter /= 2;
    }
    
    /* Make timesArray - to store search time values */
    double timesArray[numberOfArrays];
    
    /* Make threadArray of the numberOfThreads size */
    struct thread_info threadArray[numberOfThreads];
    
    fprintf(stderr,"----------------------------------------------\n");
    fprintf(stderr,"PARTITIONS:     2**n  = 2**%d = %d \n", n, numberOfPartitions);
    fprintf(stderr,"ARRAY SIZE:     2**p  = 2**%d = %d \n", p, sizeOfArray);
    fprintf(stderr,"PARTITION SIZE: %d/%d = %d\n", sizeOfArray, numberOfPartitions, partitionSize);
    fprintf(stderr,"----------------------------------------------\n");
    fprintf(stderr,"Main Program executing...\n"); 
    fprintf(stderr,"----------------------------------------------\n");
    
    /* Outputting the time before generating the array */
    gettimeofday(&time1, NULL);
    
    /* Generating random integer and storing in each array location */
    srand((int)time(NULL));
    for(i=0; i<sizeOfArray; i++){       
        //int randomNumber = rand() % 1000000000 + 1;
        int randomNumber = rand() % 1000000000 + 1;
        randomArray[i] = randomNumber;
    }
    
    /* Outputting the time after generating the array */
    gettimeofday(&time2, NULL);
    
    /* Duration to generate the array */
    generateTime = ((time2.tv_sec*1000000+time2.tv_usec)-(time1.tv_sec*1000000+time1.tv_usec))*0.000001;
    fprintf(stderr,"TIME TAKEN TO GENERATE THE ARRAY: %f seconds\n", generateTime);
    
    /* Copy numberOfArrays copies of the random Array and store into arrayOfArrays array */
    for(i=0; i<numberOfArrays; i++){
        copyArray(randomArray, sizeOfArray, arrayOfArrays[i]);
    }
    
    /* Outputting the time before sorting the array */
    gettimeofday(&time1, NULL);
    
    /* Sort 1st array without threads */
    insertionSort(randomArray,0,sizeOfArray);
    
    /* Outputting the time after sorting the array */
    gettimeofday(&time2, NULL);
    
    /**
     * Duration to sort the array (unthreaded sort) - store at index 0
     * The rest of indices will match the number of threads
     */
    timesArray[0] = ((time2.tv_sec*1000000+time2.tv_usec)-(time1.tv_sec*1000000+time1.tv_usec))*0.000001;

    
    /* Will count number of partitions from max to 1 */
    counter = numberOfPartitions;
    
    /* Will sort array number arrayNumber */
    arrayNumber = 0;
    
    /* Set global thread index - max will be total number of threads */
    globalThreadIndex = 0;

    while(counter>0){
        
        /* Get time before the sort */
        gettimeofday(&time1, NULL);

        /* Creating threads */
        for (i = 0; i < counter; i++){
            threadArray[i].threadIndex = globalThreadIndex;
            threadArray[i].localThreadIndex = i;
            threadArray[i].arrayNumber = arrayNumber;
            threadArray[i].startIndex = (sizeOfArray/counter) * i;
            threadArray[i].endIndex = (sizeOfArray/counter) * (i+1);
            if((retvalue = pthread_create(&threadArray[i].tid, NULL, (void *)threadSort, &threadArray[i])) < 0){
                perror("Can't create thread");
                exit(-1);
            }
            globalThreadIndex++;
        }
        
        /* Waiting for threads to finish */
        for(i=0; i<counter; i++){
            pthread_join(threadArray[i].tid, NULL);
        }
        
        /* Get time after sort */
        gettimeofday(&time2, NULL);
        
        /* Add the time to the timeArray */
        generateTime = ((time2.tv_sec*1000000+time2.tv_usec)-(time1.tv_sec*1000000+time1.tv_usec))*0.000001;
        timesArray[arrayNumber+1] = generateTime;
        
        /* Final sort - merge arrays */
        for (i=0; i<sizeOfArray; i++) {
            sorted[i] = get_next(counter, partitionSize,arrayOfArrays[arrayNumber],arrayOfMinArrays[arrayNumber]);
            if(i>0){
                if(sorted[i]<sorted[i-1]){
                    fprintf(stderr,"ERROR WHILE SORTING ARRAY!!! ");
                }
            }
        }
        
        counter /= 2;
        arrayNumber++;
    }

	
	/**************************************************************************/
	fprintf(stderr,"___________________________________________________\n");
    fprintf(stderr,"ARRAY SIZE: %d \n", sizeOfArray);
	fprintf(stderr,"TOTAL TIMES: \n");
    fprintf(stderr,"Duration of sequantial sort: %f seconds\n", timesArray[0]);
    counter = numberOfThreads;
    for(i=1; i<=numberOfArrays; i++){
        fprintf(stderr,"Duration of concurrent sort with %d threads: %f seconds\n", counter, timesArray[i]);
        counter /= 2;
    }

    free(minArray);
    free(randomArray);
    free(arrayOfArrays);
    free(arrayOfMinArrays);
    free(sorted);
   
}
