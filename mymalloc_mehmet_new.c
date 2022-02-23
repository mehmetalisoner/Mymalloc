#include "mymalloc.h"        
static short garbage_location = 0;  // garbage location: will used this to delete unwanted MDM blocks
static char memory[MEMSIZE];        // our "heap"/memory block

// get the start of data from a *MDM
void *get_start_of_data(struct MDM* p)          
{
    return (void*) ((void*)p + sizeof(MDM));
}


// checks for last block by checking curr.next:  -1 == last block, 0 == not last block
int is_Last(struct MDM* ptr)                    
{
    if ((char*) get_next(ptr) > &memory[MEMSIZE]) return -1;   // FIXME: MAKE SURE THIS WORKS
    else return 0;                             
}


// given a pointer MDM* ptr, get the next block with pointer arithmetic
struct MDM *get_next(struct MDM* ptr)
{
    return (MDM*) ((void*)ptr + ptr->size + sizeof(MDM));
}


// given a pointer MDM* ptr, while freeing, merging with the right free block
void check_and_merge_rightBlock(MDM* ptr)                                     
{
    if (get_next(ptr)->isFree == '1') {    // block right of curr is free
        ptr->size += get_next(ptr)->size + sizeof(MDM);
        memcpy( (void*) get_next(ptr), &garbage_location, get_next(ptr)->size + sizeof(MDM) );    // copy unwanted MDM struct "away" by copying in zero
    }
    else return;        // do nothing
}


// given a pointer MDM* ptr and MDM* prev, while freeing, merging with the left free block
void check_and_merge_leftBlock(MDM* ptr, MDM* prev) 
{
    if (prev->isFree == '1') {                                                                  // check if prev block is free
        prev->size += ptr->size + sizeof(MDM);
        memcpy( (void*) ptr, &garbage_location, ptr->size + sizeof(MDM) );
    }
    else return;        // do nothing
}


void *mymalloc(size_t size, char *file, int line)
{
    printf("Mymalloc called from %s:%d\n", file, line);
    return NULL;
}




// given a pointer p, free the data associated with p
void myfree(void *p, char *file, int line)                              
{
    printf("free called from %s:%d\n", file, line);

    if (p == NULL) {                                                                // check if pointer is a null pointer
        printf("ERROR! (FILE: %s, LINE: %d) -- INVALID POINTER! Given pointer is a NULL pointer",file,line);
        return;
    }

    if ( ((char*) p > (memory + MEMSIZE)) || ((char*) p < memory)) {                // check if pointer address is in the memory
        printf("ERROR! (FILE: %s, LINE: %d) -- INVALID POINTER! Given pointer is not in the memory",file,line);
        return;
    }

    if ((char*) memory == NULL) {                                                   // maybe an int isInit and check for that
        printf("ERROR! (FILE: %s, LINE: %d) --  MEMORY NOT INITIALIZED!",file,line);
        return;
    }


    struct MDM* curr = (struct MDM*) &memory[0];                                    // two pointers to iterate through the memory array, curr = &memory[0] also possible
    struct MDM* prev = NULL;
    unsigned short check = 0;                                                       // boolean to check if address is valid after pointer check

    // POINTER CHECK: check if given pointer has valid address

    while ( (get_next(curr) != NULL) || (p != ((void*)curr + sizeof(MDM))) ) {      // or do a helper function for start_of_data
        if (p == (void*)curr + sizeof(MDM)) {                                       // if we found the pointer in the memory
            if (curr->isFree == 1) {                                                // block is already free
                printf("ERROR! (FILE: %s, LINE: %d) --  BLOCK IS ALREADY FREE!", file ,line);
                return;
            }
            else {                                                                  // block is not free
                printf("BLOCK FOUND! CAN BE FREED NOW");
                check = 1;
                break;                                                              // hold on to the pointers by breaking
            }
        }
        prev = curr;                                                                // update pointers
        curr = get_next(curr);
    }

    if (check == 1){  // do nothing
    }
    else {
        printf("ERROR! (FILE: %s, LINE: %d) --  POINTER NOT POINTING TO THE START OF A BLOCK", file,line);
        return;
    }
    
    curr->isFree = '1';                           // free the block



    // 3 cases while merging:
    // -----------------------
    // 1. curr is in the middle randomly, have to merge 3 blocks
    // 2. curr is the last block, can only merge with left block
    // 3. curr is the first block, can only merge with the right block 



    // 1) if curr is first block, check if right block is free and merge them
    if ((char*) curr == &memory[0]) check_and_merge_rightBlock(curr);   


    
    // 2) if curr is first block, check if right block is free and merge them
    else if ( is_Last(curr) == 0) check_and_merge_leftBlock(curr,prev);


    
    // 3) block is in the middle, can merge with right and left block
    else {
        check_and_merge_rightBlock(curr);
        check_and_merge_leftBlock(curr,prev);
    }
}
