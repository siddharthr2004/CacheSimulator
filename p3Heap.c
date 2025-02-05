////////////////////////////////////////////////////////////////////////////////
// Main File:        p3Heap.c
// This File:        p3Heap.c
// Other Files:      (name of all other files if any)
// Semester:         CS 354 Summer 2024
// Instructor:       wadle
// 
// Author:           Siddharth Rajan
// Email:            sbrajan@wisc.edu
// CS Login:         siddharthr
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          
//			N/A
// Online sources:   stackoverflow.com too understand how to isolate bits 
//////////////////////////// 80 columns wide ///////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020-2024 Deb Deppeler based on work by Jim Skrentny
// Posting or sharing this file is prohibited, including any changes.
// Used by permission SPRING 2024, CS354-deppeler
//
/////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include "p3Heap.h"

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block.
 */
typedef struct blockHeader {           

    /*
     * 1) The size of each heap block must be a multiple of 8
     * 2) heap blocks have blockHeaders that contain size and status bits
     * 3) free heap block contain a footer, but we can use the blockHeader 
     *.
     * All heap blocks have a blockHeader with size and status
     * Free heap blocks have a blockHeader as its footer with size only
     *
     * Status is stored using the two least significant bits.
     *   Bit0 => least significant bit, last bit
     *   Bit0 == 0 => free block
     *   Bit0 == 1 => allocated block
     *
     *   Bit1 => second last bit 
     *   Bit1 == 0 => previous block is free
     *   Bit1 == 1 => previous block is allocated
     * 
     * Start Heap: 
     *  The blockHeader for the first block of the heap is after skip 4 bytes.
     *  This ensures alignment requirements can be met.
     * 
     * End Mark: 
     *  The end of the available memory is indicated using a size_status of 1.
     * 
     * Examples:
     * 
     * 1. Allocated block of size 24 bytes:
     *    Allocated Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 25
     *      If the previous block is allocated p-bit=1 size_status would be 27
     * 
     * 2. Free block of size 24 bytes:
     *    Free Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 24
     *      If the previous block is allocated p-bit=1 size_status would be 26
     *    Free Block Footer:
     *      size_status should be 24
     */
    int size_status;

} blockHeader;         

/* Global variable - DO NOT CHANGE NAME or TYPE. 
 * It must point to the first block in the heap and is set by init_heap()
 * i.e., the block at the lowest address.
 */
blockHeader *heap_start = NULL;     

/* Size of heap allocation padded to round to nearest page size.
 */
int alloc_size;

/*
 * Additional global variables may be added as needed below
 * TODO: add global variables needed by your function
 */




/* 
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block (payload) on success.
 * Returns NULL on failure.
 *
 * This function must:
 * - Check size - Return NULL if size < 1 
 * - Determine block size rounding up to a multiple of 8 
 *   and possibly adding padding as a result.
 *
 * - Use BEST-FIT PLACEMENT POLICY to chose a free block
 *
 * - If the BEST-FIT block that is found is exact size match
 *   - 1. Update all heap blocks as needed for any affected blocks
 *   - 2. Return the address of the allocated block payload
 *
 * - If the BEST-FIT block that is found is large enough to split 
 *   - 1. SPLIT the free block into two valid heap blocks:
 *         1. an allocated block
 *         2. a free block
 *         NOTE: both blocks must meet heap block requirements 
 *       - Update all heap block header(s) and footer(s) 
 *              as needed for any affected blocks.
 *   - 2. Return the address of the allocated block payload
 *
 *   Return if NULL unable to find and allocate block for required size
 *
 * Note: payload address that is returned is NOT the address of the
 *       block header.  It is the address of the start of the 
 *       available memory for the requesterr.
 *
 * Tips: Be careful with pointer arithmetic and scale factors.
 */
void* balloc(int size) {
    if (size < 1) {
        return NULL;
    }

    while (size % 8 != 0) {
        size++;
    }

    int totalSize = size + sizeof(blockHeader);

    blockHeader *curr = heap_start;
    blockHeader *bestFit = NULL;

    while (curr->size_status != 1) {
        int currBlockSize = curr->size_status & ~0x3;

        if ((curr->size_status & 1) == 0) { 
            if (currBlockSize >= totalSize) {
                if (bestFit == NULL || (currBlockSize - totalSize < (bestFit->size_status & ~0x3) - totalSize)) {
                    bestFit = curr;
                }
            }
        }
        curr = (blockHeader*)((char*)curr + currBlockSize);
    }

    if (bestFit == NULL) {
        return NULL; 
    }

    int bestFitSize = bestFit->size_status & ~0x3;

    if (bestFitSize == totalSize) {
        bestFit->size_status = (bestFit->size_status & ~0x1) | 1; 
        void* allocatedPtr = (void*)(((char*)bestFit) + sizeof(blockHeader));
        return allocatedPtr;
    }

    if (bestFitSize > totalSize) {
        int freeSize = bestFitSize - totalSize;
        bestFit->size_status = (bestFit->size_status & 2) | totalSize | 1; 

        blockHeader *newHeader = (blockHeader*)((char*)bestFit + totalSize);
        newHeader->size_status = freeSize | 2; 

        blockHeader *newFooter = (blockHeader*)((char*)newHeader + freeSize - sizeof(blockHeader));
        newFooter->size_status = freeSize;

        void* allocatedPtr = (void*)((char*)bestFit + sizeof(blockHeader));
        return allocatedPtr;
    }

    return NULL;
}
/* 
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - Update header(s) and footer as needed.
 *
 * If free results in two or more adjacent free blocks,
 * they will be immediately coalesced into one larger free block.
 * so free blocks require a footer (blockHeader works) to store the size
 *
 * TIP: work on getting immediate coalescing to work after your code 
 *      can pass the tests in partA and partB of tests/ directory.
 *      Submit code that passes partA and partB to Canvas before continuing.
 */                    
int bfree(void *ptr) {
    if (ptr == NULL) {
        return -1;
    }

    blockHeader *free = (blockHeader *)((char *)ptr - sizeof(blockHeader));

    if ((free->size_status & ~0x3) % 8 != 0) {
        return -1;
    }

    if ((char *)ptr >= (char *)heap_start + alloc_size || (char *)ptr < (char *)heap_start) {
        return -1;
    }

    if ((free->size_status & 1) == 0) {
        return -1;
    }

    free->size_status &= ~1;

    blockHeader *freeNext = (blockHeader *)((char *)free + (free->size_status & ~0x3));
    if ((freeNext->size_status & 1) == 0) {
        free->size_status += (freeNext->size_status & ~0x3);

        blockHeader *freeFooter = (blockHeader *)((char *)free + (free->size_status & ~0x3) - sizeof(blockHeader));
        freeFooter->size_status = free->size_status & ~0x3;

    } else {
        freeNext->size_status &= ~2; 
    }

    if ((free->size_status & 2) == 0) {
        blockHeader *freePrevFooter = (blockHeader *)((char *)free - sizeof(blockHeader));
        int prevBlockSize = freePrevFooter->size_status & ~0x3;
        blockHeader *freePrev = (blockHeader *)((char *)free - prevBlockSize);

        freePrev->size_status = (freePrev->size_status & ~0x3) + (free->size_status & ~0x3);

        blockHeader *newFooter = (blockHeader *)((char *)freePrev + (freePrev->size_status & ~0x3) - sizeof(blockHeader));
        newFooter->size_status = freePrev->size_status & ~0x3;


        free = freePrev;
    }

    blockHeader *next = (blockHeader *)((char *)free + (free->size_status & ~0x3));
    next->size_status &= ~2;

    fflush(stdout);

    return 0;
}

/* 
 * Initializes the memory allocator.
 * Called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */                    
int init_heap(int sizeOfRegion) {    

    static int allocated_once = 0; //prevent multiple myInit calls

    int   pagesize; // page size
    int   padsize;  // size of padding when heap size not a multiple of page size
    void* mmap_ptr; // pointer to memory mapped area
    int   fd;

    blockHeader* end_mark;

    if (0 != allocated_once) {
        fprintf(stderr, 
                "Error:mem.c: InitHeap has allocated space during a previous call\n");
        return -1;
    }

    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    pagesize = getpagesize();

    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    mmap_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == mmap_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }

    allocated_once = 1;

    alloc_size -= 8;

    heap_start = (blockHeader*) mmap_ptr + 1;

    end_mark = (blockHeader*)((void*)heap_start + alloc_size);
    end_mark->size_status = 1;

    heap_start->size_status = alloc_size;

    heap_start->size_status += 2;

    blockHeader *footer = (blockHeader*) ((void*)heap_start + alloc_size - 4);
    footer->size_status = alloc_size;
	

    return 0;
} 

/* STUDENTS MAY EDIT THIS FUNCTION, but do not change function header.
 * TIP: review this implementation to see one way to traverse through
 *      the blocks in the heap.
 *
 * Can be used for DEBUGGING to help you visualize your heap structure.
 * It traverses heap blocks and prints info about each block found.
 * 
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block 
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block as stored in the block header
 */                     
void disp_heap() {     

    int    counter;
    char   status[6];
    char   p_status[6];
    char * t_begin = NULL;
    char * t_end   = NULL;
    int    t_size;

    blockHeader *current = heap_start;
    counter = 1;

    int used_size =  0;
    int free_size =  0;
    int is_used   = -1;

    fprintf(stdout, 
            "********************************** HEAP: Block List ****************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, 
            "--------------------------------------------------------------------------------\n");

    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;

        if (t_size & 1) {
            // LSB = 1 => used block
            strcpy(status, "alloc");
            is_used = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "FREE ");
            is_used = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "alloc");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "FREE ");
        }

        if (is_used) 
            used_size += t_size;
        else 
            free_size += t_size;

        t_end = t_begin + t_size - 1;

        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%4i\n", counter, status, 
                p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);

        current = (blockHeader*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, 
            "--------------------------------------------------------------------------------\n");
    fprintf(stdout, 
            "********************************************************************************\n");
    fprintf(stdout, "Total used size = %4d\n", used_size);
    fprintf(stdout, "Total free size = %4d\n", free_size);
    fprintf(stdout, "Total size      = %4d\n", used_size + free_size);
    fprintf(stdout, 
            "********************************************************************************\n");
    fflush(stdout);

    return;  
} 


//		p3Heap.c (SP24) 
