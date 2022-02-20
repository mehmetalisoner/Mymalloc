#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"
#define MAXMEM 4096
static char mem[MAXMEM]={'u'};

MDM *NEXT(MDM* cpointer){
    char *ptr=(char*)cpointer;
    ptr=ptr+cpointer->size+sizeof(MDM);
    return (MDM*) ptr;
}

void *mymalloc(size_t mSize){
    if(mSize>4096){
        printf("you cannot allocate a block larger than 4096!\n");
        return NULL;
    }
    if(mSize<=0){
        printf("size cannot be zero or lower than zero\n");
        return NULL;
    }
    printf("hi\n");
    MDM *headptr = (MDM*)mem;
   
   if(mem[0]=='u'){
       headptr->allocate='0';
       headptr->size=MAXMEM;
   }
   MDM *cpoint=headptr;
    while((char*)cpoint< &mem[MAXMEM]){
        //using the cpointer to acess size from the struct 
        //we are saying if the size from the structure 
        //is greater than the size given by client and
        //we are also checking if it has been allocated yet or not.
        if(cpoint->size>=mSize && cpoint->allocate=='0'){
            //if true than we set the allocate in the structure to 1 
            cpoint->allocate='1';
            //Create a variable to store the size after subtracting it ffrom the given
            int subtractsize=cpoint->size-mSize;
            cpoint->size=mSize;
            char *rCpt = (char*)cpoint+sizeof(MDM)+mSize;
            printf("we r h\n");
            if(subtractsize<=sizeof(MDM)){
                printf("no\n");
                cpoint->size+=subtractsize;
            }
            else if(rCpt+sizeof(MDM)< &mem[MAXMEM])
            {
                printf("no2\n");
                MDM *temporary=(MDM*)rCpt;
                temporary->size = (MDM*)rCpt;
                temporary->size=subtractsize-sizeof(MDM);
                temporary->allocate='0';

            }
            else{
                printf("no3\n");
                cpoint->size+=subtractsize;
            }
            
            rCpt=(char*)cpoint+sizeof(MDM);
            printf("return>>\n");
            return (void *)rCpt;
        }else{
            cpoint=NEXT(cpoint);
        }
    }
    printf("ERROR not enogh mm");
    return NULL;
}
void printcurrmem(){
    MDM* CURR = (MDM*) mem;
    int FAGCOUNT=0;
    int BYTETR=0;
    while((char*) CURR <&mem[MAXMEM-1]){
        FAGCOUNT++;
        printf("memory frag #%d : Allocated : " , FAGCOUNT);
        if(CURR->allocate=='0'){
            printf("N, size: %d\n", CURR->size);
            
        }else{
            printf("y, size: %d\n", CURR->size);
        }
        BYTETR+=sizeof(MDM)+CURR->size;
        CURR=NEXT(CURR);}
        printf("total bytes in mem : %d\n ", BYTETR);
        printf("--------------\n");


    }
