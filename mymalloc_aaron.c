#include "mymalloc.h"   



static short garbage_location = 0x0000;  // garbage location: will used this to delete unwanted MDM blocks
static char memory[MEMSIZE];        // our "heap"/memory block
signed short is_Init = -1;         // checks if memory is initialized: -1 == is not, 0 == is initialized


void *mymalloc(size_t mSize, char *file, int line)
{
    printf("Mymalloc called from %s:%d\n", file, line);

    // Errors:
    if(mSize > MEMSIZE){
        printf("ERROR! (FILE: %s, LINE: %d) -- You cannot allocate a block larger than 4096!\n",file,line);
        return NULL;
    }
    if(mSize <= 0){
        printf("ERROR! (FILE: %s, LINE: %d) -- Size cannot be zero or lower than zero\n",file,line);
        return NULL;
    }
    MDM *head = (MDM*) &memory[0];
   
    if(is_Init == -1){
       head->isFree = '0';
       head->size = MEMSIZE;
       is_Init = 0;
    }

    MDM *ptr = head;
    while( (char*)ptr < &memory[MEMSIZE]){
        if(ptr->size >= mSize && ptr->isFree == '0'){
            ptr->isFree = '1';                          // not free anymore
            int leftover_Memory = ptr->size - mSize;
            ptr->size = mSize;
            char *rigth_Block = (char*) ptr + sizeof(MDM)+ mSize;
            if(leftover_Memory <= sizeof(MDM)){
                ptr->size+=leftover_Memory;
            }
            else if(rigth_Block + sizeof(MDM)< &memory[MEMSIZE])
            {
                MDM *temp = (MDM*) rigth_Block;
                temp->size = leftover_Memory - sizeof(MDM);
                temp->isFree = '0';
            }
            void* result = get_start_of_data(ptr);
            return result;
        }
        else {
            ptr = get_next(ptr);
        }
    }
    printf("ERROR! (FILE: %s, LINE: %d) -- Not enough memory\n ",file,line);
    return NULL;
}



// get the start of data from a *MDM
void *get_start_of_data(struct MDM* p)          
{
    return (void*) ((char*)p + sizeof(MDM));
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
    return (MDM*) ((char*)ptr + ptr->size + sizeof(MDM));
}


// given a pointer MDM* ptr, while freeing, merging with the right free block
void check_and_merge_rightBlock(MDM* ptr)                                     
{
    if (get_next(ptr)->isFree == '0') {    // block right of curr is free
        MDM* right_Block = get_next(ptr);
        ptr->size += right_Block->size + sizeof(MDM);
        memcpy( (void*) right_Block, &garbage_location, right_Block->size + sizeof(MDM) );    // copy unwanted MDM struct "away" by copying in zero
        printf("BLOCK FREED\n");
    }
    else {
        memcpy( (void*) ptr, &garbage_location, ptr->size + sizeof(MDM));
        printf("BLOCK FREED\n");
    }
}


// given a pointer MDM* ptr and MDM* prev, while freeing, merging with the left free block
void check_and_merge_leftBlock(MDM* ptr, MDM* prev) 
{
    if (prev->isFree == '0') {                                                                  // check if prev block is free
        prev->size += ptr->size + sizeof(MDM);
        memcpy( (void*) ptr, &garbage_location, ptr->size + sizeof(MDM) );
        printf("BLOCK FREED\n");
    }
    else {
        MDM* temp = ptr;
        memcpy( (void*) temp, &garbage_location, temp->size + sizeof(MDM));
        printf("BLOCK FREED\n");
    } // FIXME: FREE BLOCK IF PREV IS NOT FREE
}


// given a pointer p, free the data associated with p
void myfree(void *p, char *file, int line)                              
{
    printf("Free called from %s:%d\n", file, line);

    if (p == NULL) {                                                                // check if pointer is a null pointer
        printf("ERROR! (FILE: %s, LINE: %d) -- INVALID POINTER! Given pointer is a NULL pointer\n",file,line);
        return;
    }

    if ( ((char*) p > (memory + MEMSIZE)) || ((char*) p < memory)) {                // check if pointer address is in the memory
        printf("ERROR! (FILE: %s, LINE: %d) -- INVALID POINTER! Given pointer is not in the memory\n",file,line);
        return;
    }

    if ((char*) memory == NULL) {                                                   // maybe an int isInit and check for that
        printf("ERROR! (FILE: %s, LINE: %d) --  MEMORY NOT INITIALIZED!\n",file,line);
        return;
    }


    struct MDM* curr = (MDM*) &memory[0];                                    // two pointers to iterate through the memory array, curr = &memory[0] also possible
    struct MDM* prev = NULL;
    unsigned short check = 0;                                                       // boolean to check if address is valid after pointer check

    // POINTER CHECK: check if given pointer has valid address

    while ( (get_next(curr) != NULL) || (p != get_start_of_data(curr))) {      // or do a helper function for start_of_data
        if (p == get_start_of_data(curr)) {                                       // if we found the pointer in the memory
            if (curr->isFree == '0') {                                                // block is already free
                printf("ERROR! (FILE: %s, LINE: %d) --  BLOCK IS ALREADY FREE!\n", file ,line);
                return;
            }
            else {                                                                  // block is not free
                printf("BLOCK FOUND! CAN BE FREED NOW\n");
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
        printf("ERROR! (FILE: %s, LINE: %d) --  POINTER NOT POINTING TO THE START OF A BLOCK\n", file,line);
        return;
    }
    
    curr->isFree = '0';                           // free the block



    // 3 cases while merging:
    // -----------------------
    // 1. curr is in the middle randomly, have to merge 3 blocks
    // 2. curr is the last block, can only merge with left block
    // 3. curr is the first block, can only merge with the right block 



    // 1) if curr is first block, check if right block is free and merge them
    if ( (char*)curr == &memory[0] ) check_and_merge_rightBlock(curr);   


    
    // 2) if curr is first block, check if right block is free and merge them
    else if ( is_Last(curr) == 0) check_and_merge_leftBlock(curr,prev);


    
    // 3) block is in the middle, can merge with right and left block
    else {
        check_and_merge_rightBlock(curr);
        check_and_merge_leftBlock(curr,prev);
    }
}
