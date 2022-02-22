#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mymalloc.h"
#define MEMSIZE 4096                            // memory size macro


static short garbage_location = 0;              // will be used to zero out/delete unused MDM blocks
static char memory[MEMSIZE];                    // our "heap"/memory block

struct MDM {                                    // metadata class, this is how we will store our data into memory
    struct MDM *prev, *next;
    int isFree;                                 // if usage == 1, then its free, if usage == 0, its occupied
    unsigned int size;
};

static struct MDM *global_start = NULL;         // this will be a global pointer that we can refer to. 
                                                // initially it will be the first block but after mallocing, it will be the last

void init_memory(){                             // this is basically putting in the first block or initializing the memory array
    global_start = (struct MDM*) &memory[0];
    global_start->next = NULL;                  // this will always be NULL since this will be the last block
    global_start->prev = NULL;
    global_start->isFree = 1;
    global_start->size = MEMSIZE - sizeof(struct MDM);
}

void *get_start_of_data(struct MDM* p)          // might not need this
{
    return (void*)p + sizeof(struct MDM);
}

int is_Last(struct MDM* ptr)                    // checks for last block by checking curr.next; return -1 if last block, 0 if not last block
{
    if (ptr->next == NULL) return -1;   
    else return 0;                             
}

int pointer_check(void *p)                                              // find *p in array and see if it points to the start of a block. -1 == not valid, 0 == valid
{
    struct MDM* curr = global_start;                                    // two pointers to iterate through the memory array, curr = &memory[0] also possible
    struct MDM* prev = NULL;

    while ((curr->next != NULL) || (p != get_start_of_data(curr))) {    
        if (p == get_start_of_data(curr)) {                             // if we found the pointer in the memory at the start of a block
            if (curr->isFree == 1) {                                    // block is already free
                printf("BLOCK IS ALREADY FREE!");
                return -1;
            }
            else {                                                      // block is not free
                printf("BLOCK FOUND! CAN BE FREED NOW");
                return 0;
            }
        }
        prev = curr;                                                    // update pointers
        curr = curr->next;
    }
    printf("POINTER NOT POINTING TO THE START OF A BLOCK");
    return -1;
}

void coalesce(struct MDM *curr, struct MDM *prev)                                                          // coalesce with 3 free blocks, curr pointer in the middle
{
    prev->next = curr->next->next;                                                                         // update pointers
    prev->size += curr->size + curr->next->size + 2 * sizeof(struct MDM);                                  // update size
    if (is_Last(curr->next) == 0)   curr->next->next->prev = prev;                                         // curr.next.next isnt't last block adjust pointers, if not then do nothing                                         
    memcpy((void*)curr,&garbage_location,(curr->size + curr->next->size + 2 * sizeof(struct MDM)));        // copy curr and curr.next to garbage location
    return;
}

void *mymalloc(size_t size, char *file, int line)
{
    printf("Mymalloc called from %s:%d\n", file, line);
    return NULL;
}

void myfree(void *p, char *file, int line)                              // given a VOID pointer p, free the data associated with p
{
    printf("free called from %s:%d\n", file, line);

    if (p == NULL) {                                                    // check if pointer is a null pointer
        printf("INVALID POINTER! Given pointer is a null pointer");
        return;
    }

    if ((p > (void*)(memory + MEMSIZE)) || (p < (void*)memory)) {       // check if pointer address is in the memory
        printf("INVALID POINTER! Given pointer is not in the memory");
        return;
    }

    if ((void*) memory == NULL) {                                       // maybe an int isInit and check for that
        printf("MEMORY NOT INITIALIZED!");
        return;
    }


    // Note: if the pointer passes these edge cases, we still have to check if the pointer is pointing
    // to the start of the block OR if the block that it is pointing to is already free

    
    if (pointer_check == -1) return;            // pointer is not pointing to a valid address, exit program
    struct MDM* curr = (struct MDM*) p;         // current pointer pointing to p, the block we want to free
    struct MDM* prev = curr->prev;              // previous pointer, the block before curr
    curr->isFree = 1;                           // free the block, mark as free
    
    
    
    // Edge cases when merging blocks:
    // ---------------------------
    // 1. prev block is free
    // 2. curr.next block is free
    // 3. both are free
    
    
    // Note:
    // we still have to check if .next is the last block because of a NULL pointer exception
    // by design we don't do any merging if we are freeing the first or the last block in the memory array
    // they can be merged if the block next to them gets freed
    
    
    // Checking for NULL in order to avoid NULL pointer exception
    if ( (prev->isFree == 1) && (prev != NULL) && (curr->next->isFree == 1) && (curr->next != NULL) ) { // both are free and not NULL
        coalesce(curr,prev); // coalesce all three blocks
        return;
    }

    else if ( (prev->isFree == 1) && (prev != NULL) ) {     // if only previous is free and not NULL
        prev->next = curr->next;
        prev->size += curr->size + sizeof(struct MDM);
        if (is_Last(curr) == 0) curr->next->prev = prev;    // do this if curr is not last block
        memcpy((void*)curr,&garbage_location, curr->size + sizeof(struct MDM));         // zeroing unwanted MDM
        return;
    }

    else if ( (curr->next->isFree == 1) && (curr->next != NULL) ) {   // if only .next is free and not NULL
        curr->next = curr->next->next;
        curr->next->next->prev = curr; 
        curr->size += curr->next->size + sizeof(struct MDM);
        // if(is_Last(curr->next) == 0) curr->next->next->prev = curr;                          // don't need to check for this because of if statement
        memcpy((void*)curr->next, &garbage_location,curr->next->size + sizeof(struct MDM));     // zeroing unwanted MDM
        return;
    }

}
