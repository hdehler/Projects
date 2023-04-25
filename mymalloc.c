// Harrison Dehler
// COSC 360
// Creating malloc through free list of memory
#include "mymalloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// chunk struct representing a block of memory
typedef struct Chunk {
    size_t size;
    struct Chunk *next;
} chunk;

// keep track of the free list
chunk *head = NULL;

// function to allocate memory
void *my_malloc(size_t size) {
    if (size <= 0) {
        return NULL;
    }

    // calculate the size of the chunk including padding
    size_t chunkSize = (size + 7 + 8) & -8;

    // set the minimum size for a block of memory obtained from sbrk
    size_t sbrkMin = 8192;
   

    // if the chunk size is larger than sbrkMin, request new memory using sbrk
    if (chunkSize > sbrkMin) {
        chunk *c = sbrk(chunkSize);
        c->size = chunkSize;
        // return a pointer to the allocated block of memory
        return ((void *)c + 8);
    }

    // search for a free block of memory in the free list
    chunk *curr = head;
    chunk *p = NULL;
    while (curr != NULL) {
        if (curr->size >= chunkSize) {
            break;
        }
        p = curr;
        curr = curr->next;
    }

    // if no free block was found, request new memory using sbrk
    if (curr == NULL) {
        chunk *c = sbrk(sbrkMin);
        c->size = 8192 - chunkSize;
        chunk *tmp = (void *)c + (c->size);
        tmp->size = chunkSize;
        // free the unused block of memory
        my_free(((void *)c + 8));
        // return a pointer to the allocated block of memory
        return ((void *)tmp + 8);
    }

    // if a free block was found, allocate the memory and update the free list
    // and return a pointer to allocated block of memory
    if (curr->size - chunkSize > 8) {
        curr->size -= chunkSize;
        chunk *tmp = ((void *)curr + curr->size);
        tmp->size = chunkSize;
        return ((void *)tmp + 8);
    } else {
        // remove the free block from the free list
        chunk *find = head;
        if (find->next == NULL) {
            head = NULL;
            curr->size = chunkSize;
            return ((void *)curr + 8);
        }
        while (find->next != curr) {
            find = find->next;
        }
        find->next = curr->next;
        curr->size = chunkSize;
        return ((void *)curr + 8);
    }
}

void my_free(void *ptr) {
    // Get the chunk corresponding to the pointer to be freed
    chunk *block = ptr - 8;

    // If the free list is empty, add the block as the first element
    if (head == NULL) {
        head = block;
        head->next = NULL;
        return;
    }

    // Traverse the free list to find the position to insert the block
    chunk *prev = NULL;
    chunk *curr = head;
    while (curr != NULL && curr < block) {
        prev = curr;
        curr = curr->next;
    }

    // Insert the block at the appropriate position
    if (prev == NULL) {  // Insert at the beginning
        block->next = curr;
        head = block;
    } else if (curr == NULL) {  // Insert at the end
        prev->next = block;
        block->next = NULL;
    } else {  // Insert in the middle
        prev->next = block;
        block->next = curr;
    }
}

void *free_list_begin() { return head; }

void *free_list_next(void *curr) { return ((chunk *)curr)->next; }

void coalesce_free_list() {
    chunk *curr = head;
    while (curr != NULL) {
        while ((void *)curr + curr->size == ((void *)(curr->next))) {
            curr->size += curr->next->size;
            curr->next = curr->next->next;
        }
        curr = curr->next;
    }
}
