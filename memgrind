#include "mymalloc.h"
#include <sys/time.h>
#include <time.h>
#define ITERATION_COUNT 50


long test_1(size_t bytes)                                   // malloc and immediately free a n-byte chunk
{
    struct timeval start, end;
    gettimeofday(&start, NULL); 
    for (int i = 0; i < 120; i++) {
        char*p = malloc(bytes);
        free(p);
    }
    gettimeofday(&end, NULL);
    return (end.tv_usec - start.tv_usec);
}

long test_2(size_t bytes)                                   // allocte 120 n-byte chunks, store them in an array and free them after  
{
    struct timeval start, end;
    gettimeofday(&start, NULL); 
    void* ptrs[120];
    for (int i = 0; i < 120; i++) {
        void*p = malloc(bytes);
        ptrs[i] = p;
    }
    for (int i = 0; i < 120; i++) {
        free(ptrs[i]);
    }
    gettimeofday(&end, NULL);
    return (end.tv_usec - start.tv_usec);       
}


long test_3(size_t bytes) 
{
    int times_allocated = 0;
    int times_stored = 0;
    void* ptrs[120];
    struct timeval start, end;
    gettimeofday(&start, NULL); 
    while (times_allocated < 120) {                  // keep mallocing until you have malloced 120-times
        srand(time(0));
        int random = (rand() % 2);      // create a random number. either 0 or 1

        if (random == 0) {              // if random == 0, then malloc and store the pointer
            void*p = malloc(bytes);
            if (p == NULL) continue;
            ptrs[times_allocated] = p;
            times_allocated++;
            times_stored++;
        }
        else {
            if (times_stored > 0) {
                free(ptrs[times_allocated-1]); // free the last pointer available
                times_stored--;             // decrement the amount of pointers stored
            }
        }
    }
    if (times_stored > 0 ) {
        for (int j = 0; j < times_stored-1; j++) {
            free(ptrs[j]);
        }
    }
    gettimeofday(&end, NULL); 
    return ((end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec);       
      
}



long test_4(size_t bytes) 
{
    void* ptrs[120];
    struct timeval start, end;
    int i = 0;
    gettimeofday(&start, NULL); 
    while (i  < 120) {                  // keep mallocing until you have malloced 120-times
        srand(time(0));
        int random = (rand() % 2);      // create a random number. either 0 or 1

        if (random == 0) {              // if random == 0, then malloc and store the pointer
            void*p = malloc(bytes);
            // if (p == NULL) continue;
            ptrs[i] = p;
        }
        else {
            void*p = malloc(29);
            // if (p == NULL) continue;
            ptrs[i] = p;
        }
        i++;
    }
    for (int j = 0; j < 120; j++) {
        free(ptrs[j]);
    }
    gettimeofday(&end, NULL); 

    return ((end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec);       

}

long test_5(size_t bytes)
{


    void* ptrs[120];
        struct timeval start, end;
        int i = 0;
        gettimeofday(&start, NULL); 
        while (i  < 120) {                  // keep mallocing until you have malloced 120-times
            srand(time(0));
            int random = (rand() % 2);      // create a random number. either 0 or 1

            if (random == 0) {              // if random == 0, then malloc and store the pointer
                void*p = malloc(bytes);
                // if (p == NULL) continue;
                ptrs[i] = p;
            }
            else {
                void*p = malloc(29);
                // if (p == NULL) continue;
                ptrs[i] = p;
            }
            i++;
        }
        for (int j = 0; j < 120; j++) {
            free(ptrs[j]);
        }
        gettimeofday(&end, NULL); 

        return ((end.tv_sec - start.tv_sec)*1000000 + end.tv_usec - start.tv_usec);  

}


long get_average(long times_testn[])
{
    long average = 0;
    for (int i = 0; i < ITERATION_COUNT; i++) {
        average += times_testn[i];
    }
    return average/ITERATION_COUNT;
}



int main() {
    // create arrays to store the times for each test case
    long times_test1[ITERATION_COUNT];
    long times_test2[ITERATION_COUNT];
    long times_test3[ITERATION_COUNT];
    long times_test4[ITERATION_COUNT];
    long times_test5[ITERATION_COUNT];



    for (int k = 0; k < ITERATION_COUNT; k++) {
        times_test1[k] = test_1(1);
        times_test2[k] = test_2(1);
        times_test3[k] = test_3(1);
        times_test4[k] = test_4(1);
        times_test5[k] = test_5(1);
    }
    

    printf("Total time for test 1 in microseconds: %ld\n",get_average(times_test1));

    printf("Total time for test 2 in microseconds: %ld\n",get_average(times_test2));
       
    printf("Total time for test 3 in microseconds: %ld\n",get_average(times_test3));

    printf("Total time for test 4 in microseconds: %ld\n",get_average(times_test4));
    
    printf("Total time for test 5 in microseconds: %ld\n",get_average(times_test5));



    

    return 0;
}
