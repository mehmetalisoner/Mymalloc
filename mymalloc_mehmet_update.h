#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MEMSIZE 4096 
#define malloc(s) mymalloc(s, __FILE__, __LINE__)
#define free(p) myfree(p, __FILE__, __LINE__)


struct MDM {                        // metadata class, this is how we will store our data into memory
    // struct MDM *prev, *next;
    // void* start_of_data;
    char isFree;                     // free == '1', not free == '0'.
    unsigned int size;
};

typedef struct MDM MDM;
void *mymalloc(size_t size, char *file, int line);
void myfree(void *p, char *file, int line);
void *get_start_of_data(struct MDM* p);
int is_Last(struct MDM* p);
struct MDM *get_next(struct MDM* ptr);
void check_and_merge_rightBlock(MDM* ptr);                                   
void check_and_merge_leftBlock(MDM* ptr, MDM* prev); 
