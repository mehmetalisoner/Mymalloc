#include "mymalloc.h"   

static char memory[MEMSIZE];        // our "heap"/memory block
signed short is_Init = -1;         // checks if memory is initialized: -1 == is not, 0 == is initialized


void *mymalloc(size_t mSize, char *file, int line)
{
    //printf("Mymalloc called from %s:%d\n", file, line);

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
       head->size = MEMSIZE-sizeof(MDM);
       is_Init = 0;
    }

    MDM *ptr = head;
    while( (char*)ptr < &memory[MEMSIZE]){      
        if(ptr->size >= mSize && ptr->isFree == '0'){                   // if we found a block that can hold our wanted struct
            ptr->isFree = '1';                                          // not free anymore
            int leftover_Memory = ptr->size - mSize;                    // subtract the size of inserted bytes from the main free memory
            ptr->size = mSize;                                          // update size of wanted block to mSize. NOTE: We are NOT adding sizeof(MDM) because we are checking for it after
            char *rigth_Block = (char*) ptr + sizeof(MDM)+ mSize;       
            if(leftover_Memory <= sizeof(MDM)){                         // if the leftover memory cannot hold an MDM struct
                if (is_Last(ptr) == 0) ptr->size+=leftover_Memory;      // if ptr is the last block, then undo and add mSize back to the main free memory
                else ptr->size += leftover_Memory;                      // if not, put in more bytes into ptr in order to maintain memory structure
            }
            else if(rigth_Block + sizeof(MDM)< &memory[MEMSIZE])        // if we have enough space then
            {
                MDM *temp = (MDM*) rigth_Block;                         // create right block, temp will become the rest of the main free memory
                temp->size = leftover_Memory - sizeof(MDM);             // update size
                temp->isFree = '0';                                     // mark as free
            }
            void* result = get_start_of_data(ptr);                      // we are done, return the start of data of ptr
            return result;
        }
        else {
            ptr = get_next(ptr);                                        // else, keep on searching for the right block
        }
    }
    // printf("ERROR! (FILE: %s, LINE: %d) -- Not enough memory\n ",file,line);
    return NULL;
}



// get the start of data from a *MDM
void *get_start_of_data(struct MDM* p)          
{
    return (void*) ((char*)p + sizeof(MDM));
}


// checks for last block by checking curr.next:  0 == last block, -1 == not last block
int is_Last(struct MDM* ptr)                    
{
    if (((char*) ptr + ptr->size + sizeof(MDM)) > &memory[MEMSIZE]) return 0;   // FIXME: MAKE SURE THIS WORKS
    else return -1;                             
}


// given a pointer MDM* ptr, get the next block with pointer arithmetic
struct MDM *get_next(struct MDM* ptr)
{
    return (MDM*) ((char*)ptr + ptr->size + sizeof(MDM));
}

// given a pointer p, free the data associated with p
void myfree(void *p, char *file, int line)                              
{
    //printf("Free called from %s:%d\n", file, line);

    if (p == NULL) {                                                                // check if pointer is a null pointer
        //printf("ERROR! (FILE: %s, LINE: %d) -- INVALID POINTER! Given pointer is a NULL pointer\n",file,line);
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


    struct MDM* curr = (MDM*) &memory[0];                                           // two pointers to iterate through the memory array, curr = &memory[0] also possible
    struct MDM* prev = NULL;
    unsigned short check = 0;                                                       // boolean to check if address is valid after pointer check

    // POINTER CHECK: check if given pointer has valid address

    while ( (get_next(curr) != NULL) || (p != get_start_of_data(curr))) {           // or do a helper function for start_of_data
        if (p == get_start_of_data(curr)) {                                         // if we found the pointer in the memory
            if (curr->isFree == '0') {                                                // block is already free
                //printf("ERROR! (FILE: %s, LINE: %d) --  BLOCK IS ALREADY FREE!\n", file ,line);
                return;
            }
            else {                                                                  // block is not free
                //printf("BLOCK FOUND! CAN BE FREED NOW\n");
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
        //printf("ERROR! (FILE: %s, LINE: %d) --  POINTER NOT POINTING TO THE START OF A BLOCK\n", file,line);
        return;
    }
    
    curr->isFree = '0';                           // free the block



    // 3 cases while merging:
    // -----------------------
    // 1. curr is in the middle randomly, have to merge 3 blocks
    // 2. curr is the last block, can only merge with left block
    // 3. curr is the first block, can only merge with the right block 



    // 1) if curr is first block, check if right block is free and merge them
    if ( (char*)curr == &memory[0] ) {
        if (get_next(curr)->isFree == '0') {    // block right of curr is free
            MDM* right_Block = get_next(curr);
            curr->size += right_Block->size + sizeof(MDM);
        }
    } 


    
    // 2) if curr is first block, check if right block is free and merge them
    else if ( is_Last(curr) == 0){
        if (prev->isFree == '0') {                                                                  // check if prev block is free
            prev->size += curr->size + sizeof(MDM);
        }
    }


    
    // 3) block is in the middle, can merge with right and left block
    else {
        if (get_next(curr)->isFree == '0') {    // block right of curr is free
            MDM* right_Block = get_next(curr);
            curr->size += right_Block->size + sizeof(MDM);
        }
        if (prev->isFree == '0') {                                                                  // check if prev block is free
            prev->size += curr->size + sizeof(MDM);
        }
    }
    //printf("BLOCK FREED\n");

}
