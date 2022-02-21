#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mymalloc.h"
#define MEMSIZE 4096                // memory size macro


static short garbage_location = 0xABCD;
static char memory[MEMSIZE];        // our "heap"/memory block

struct MDM {                        // metadata class, this is how we will store our data into memory
    struct MDM *prev, *next;
    int isFree;                     // if usage == 1, then its free, if usage == 0, its occupied
    unsigned int size;
};

static struct MDM *global_start = NULL;     // this will be a global pointer that we can refert to. 
                                            // initially it will be the first block but after mallocing, it will be the last

void init_memory(){
    global_start = (struct MDM*) memory[0];
    global_start->next = NULL;    // this will always be true since this will be the last block
    global_start->prev = NULL;
    global_start->isFree = 1;
    global_start->size = MEMSIZE - sizeof(struct MDM);
}

void *get_start_of_data(struct MDM* p)          // might not need this
{
    return (void*)p + sizeof(struct MDM);
}

int is_Last(struct MDM* ptr)                    // checks for last block by checking curr.next, -1 == last block, 0 == not last block
{
    if (ptr->next == NULL) return -1;   
    else return 0;                             
}

int pointer_check(void *p)                                              // find *p in array, if we return NULL we have error, if we return a pointer
{
    struct MDM* curr = global_start;                                    // two pointers to iterate through the memory array, curr = &memory[0] also possible
    struct MDM* prev = NULL;

    while ((curr->next != NULL) || (p != get_start_of_data(curr))) {    // or do a helper function for start_of_data
        if (p == get_start_of_data(curr)) {                             // if we found the pointer in the memory
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

void coalesce(struct MDM *curr, struct MDM *prev)                                                          // coalesce with 3 free blocks, curr in the middle
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

void myfree(void *p, char *file, int line)                              // given a pointer p, free the data associated with p
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

    
    if (pointer_check == -1) return;            // pointer is not pointing to a valid address
    struct MDM* curr = (struct MDM*) p;         // current pointer
    struct MDM* prev = curr->prev;              // previous pointer
    curr->isFree = 1;                           // free the block

    // If there is coalesing, then there are two three cases:
    // 1. prev block is free
    // 2. curr.next block is free
    // 3. both are free
    // we still have to check if .next is the last block because of a NULL pointer exception
    // by design we don't do any merging if we are freeing the first or the last block
    // they can be merged if the block next to them gets freed

    if (((prev->isFree == 1) && (prev != NULL)) && ((curr->next->isFree == 1) && (curr->next != NULL))) { // both are free and not NULL, do this instead: temp.next != LAST_BLOCK
        coalesce(curr,prev); // coalesce all three blocks
        return;
    }

    else if ((prev->isFree == 1) && (prev != NULL)) {     // if only previous is free and not null
        prev->next = curr->next;
        prev->size += curr->size + sizeof(struct MDM);
        if (is_Last(curr) == 0) curr->next->prev = prev;
        memcpy((void*)curr,&garbage_location, curr->size + sizeof(struct MDM));
        return;
    }

    else if ((curr->next->isFree == 1) && (curr->next != NULL)) {   // if only .next is free and not null
        curr->next = curr->next->next;
        curr->next->next->prev = curr; 
        curr->size += curr->next->size + sizeof(struct MDM);
        // if(is_Last(curr->next) == 0) curr->next->next->prev = curr;      // don't need to check for this because of if statement
        memcpy((void*)curr->next, &garbage_location,curr->next->size + sizeof(struct MDM));
        return;
    }

}
