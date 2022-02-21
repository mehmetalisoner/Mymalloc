#include <stdio.h>
#define malloc(s) mymalloc(s, __FILE__, __LINE__)
#define free(p) myfree(p, __FILE__, __LINE__)


struct MDM {                        // metadata class, this is how we will store our data into memory
    struct MDM *prev, *next;
    // void* start_of_data;
    int isFree;                     // if usage == 1, then its free, if usage == 0, its occupied
    unsigned int size;
};

void *mymalloc(size_t size, char *file, int line);
void myfree(void *p, char *file, int line);
void init_memory();
void *get_start_of_data(struct MDM* p);
int is_Last(struct MDM* p);
int pointer_check(void *p);
void coalesce(struct MDM *temp, struct MDM *prev);
