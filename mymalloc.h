#include <stdlib.h>
#include <stdio.h>


typedef struct MDM{
   
    unsigned int size;
    int isfree;
    char allocate;
}MDM;
    


void *mymalloc(size_t size);

MDM* NEXT(MDM *cpointer);
