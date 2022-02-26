#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MEMSIZE 4096 
#define malloc(s) mymalloc(s, __FILE__, __LINE__)
#define free(p) myfree(p, __FILE__, __LINE__)


struct MDM {                         // metadata class, this is how we will store our data into memory
    unsigned int size;               // 4 bytes
    char isFree;                     // not free == '1', free == '0'. 1 bytes
};

typedef struct MDM MDM;
void *mymalloc(size_t size, char *file, int line);
void myfree(void *p, char *file, int line);
void *get_start_of_data(MDM* p);
int is_Last(MDM* p);
struct MDM *get_next(struct MDM* ptr);
