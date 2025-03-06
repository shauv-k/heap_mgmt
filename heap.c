#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define HEAP_SIZE 4096

typedef struct Block {
    int size;
    bool is_free;
    struct Block *next;
    struct Block *free_next;
} Block;

Block *heap = NULL; 
Block *free_list = NULL; 

int next_power_of_2(int n) {
    int power = 1;
    while (power < n) {
        power *= 2;
    }
    return power;
}

Block* init_heap() {
    heap = (Block*) malloc(sizeof(Block));
    if (!heap) {
        printf("Heap initialization failed!\n");
        return NULL;
    }
    heap->size = HEAP_SIZE;
    heap->is_free = true;
    heap->next = NULL;
    heap->free_next = NULL;
    free_list = heap;
    return heap;
}



Block* allocate(int size) {
    size = next_power_of_2(size); // Ensure power of 2 allocation
    Block *current = free_list, *prev = NULL;

    // Search for a suitable free block
    while (current) {
        if (current->is_free && current->size >= size) {
            // Keep splitting until the block size matches
            while (current->size > size) { 
                int new_size = current->size / 2;
                Block *buddy = (Block*) malloc(sizeof(Block));
                if (!buddy) {
                    printf("Memory allocation failed!\n");
                    return NULL;
                }
                buddy->size = new_size;
                buddy->is_free = true;
                buddy->next = current->next;
                buddy->free_next = current->free_next;

                // Update current block
                current->size = new_size;
                current->next = buddy;
                current->free_next = buddy;
            }

            // Mark as allocated
            current->is_free = false;

            // Remove from free list
            if (prev) prev->free_next = current->free_next;
            else free_list = current->free_next;
            current->free_next = NULL;

            return current;
        }
        prev = current;
        current = current->free_next;
    }

    printf("No suitable block found.\n");
    return NULL;
}



void merge_free_blocks() {
    Block *current = heap;
    while (current && current->next) {
        if (current->is_free && current->next->is_free && current->size == current->next->size) {
            Block *buddy = current->next;
            current->size *= 2;
            current->next = buddy->next;
            current->free_next = buddy -> free_next;
            buddy->next = NULL;
            buddy->free_next = NULL;
            free(buddy);
            buddy = NULL; 
        } else {
            current = current->next;
        }
    }
}

void deallocate(Block *block) {
    if (!block) 
    {
        printf("Unallocated Block\n");
        return;
    }

    block->is_free = true;  // Mark block as free

    // Step 1: Find next free block in the heap
    Block *next_free = block->next;
    while (next_free && !next_free->is_free) {
        next_free = next_free->next;
    }

    // Step 2: Find previous free block in the free list
    Block *prev_free = NULL, *current = free_list;
    while (current && current != next_free) {
        prev_free = current;
        current = current->free_next;
    }

    // Step 3: Insert block into the free list
    if (prev_free) {
        prev_free->free_next = block;
    } else {
        free_list = block;  // If no previous, block is new head
    }
    block->free_next = next_free;

    // Step 4: Merge adjacent free blocks
    merge_free_blocks();
}


// Print heap status
void print_heap() {
    Block *current = heap;
    printf("Heap blocks:\n");
    while (current) {
        printf("[%s | Size: %d] -> ", current->is_free ? "Free" : "Alloc", current->size);
        current = current->next;
    }
    printf("NULL\n");
}

void print_free() {
    Block *current = free_list;
    printf("Free blocks:\n");
    while (current) {
        printf("[%s | Size: %d] -> ", current->is_free ? "Free" : "Alloc", current->size);
        current = current->free_next;
    }
    printf("NULL\n");
}

int main() {
    init_heap();
    Block *ptr1 = allocate(200);
    Block *ptr2 = allocate(50);
    Block *ptr3 = allocate(500);
    Block *ptr4 = allocate(5000);
    //Block *ptr5 = allocate(200);
    print_heap();
    print_free();

    deallocate(ptr1);
    //print_free();
    deallocate(ptr2);
    //print_free();
    deallocate(ptr3); 
    print_heap();
    print_free();

    return 0;
}
