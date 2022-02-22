#include <stdlib.h>
#include <stdio.h>


typedef struct MDM{
   
    unsigned int size;
    int isfree;
    char allocate;         // Question: is this same int isFree? If yes, then we can just get delete int isFree
}MDM;
    


void *mymalloc(size_t size);

MDM* NEXT(MDM *cpointer);
