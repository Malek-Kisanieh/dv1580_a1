#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

// Structure for managing a block within the memory pool
typedef struct MemBlock {
    size_t block_size;       // Size of the block
    int is_available;        // 1 if available, 0 if in use
    struct MemBlock* next_block;  // Link to the next block in the pool
    void* data_ptr;          // Pointer to the start of usable data in the block
} MemBlock;

void* pool_start = NULL;       // Beginning of the memory pool
MemBlock* pool_head = NULL;    // Head of the linked list of memory blocks
size_t total_pool_size = 0;    // Size of the entire memory pool

// Initialize the memory pool to a given size
// Parameters:
// - pool_size: size to allocate for the pool
// Error handling:
// - Prints an error message if memory allocation fails and exits.
void mem_init(size_t pool_size) {
    pool_start = malloc(pool_size);
    if (!pool_start) {
        perror("Failed to allocate memory pool");
        exit(EXIT_FAILURE);
    }

    total_pool_size = pool_size;

    // Set up the first block as the entire pool
    pool_head = (MemBlock*)malloc(sizeof(MemBlock));
    if (!pool_head) {
        perror("Failed to create block metadata");
        free(pool_start);
        exit(EXIT_FAILURE);
    }

    pool_head->block_size = pool_size;
    pool_head->is_available = 1;        // Initially, the entire pool is free
    pool_head->data_ptr = pool_start;   // Points to the start of the memory pool
    pool_head->next_block = NULL;
}

// Allocate a block within the pool of the specified size
// Parameters:
// - size: the size of memory to allocate
// Returns:
// - A pointer to the allocated block if successful, or NULL if no suitable block exists
void* mem_alloc(size_t size) {
    MemBlock* current = pool_head;

    // Traverse to find a suitable free block
    while (current != NULL) {
        if (current->is_available && current->block_size >= size) {
            if (current->block_size > size) {
                // Create a new block for remaining free space
                MemBlock* new_block = (MemBlock*)malloc(sizeof(MemBlock));
                if (!new_block) {
                    perror("Failed to create new block metadata");
                    return NULL;
                }

                new_block->block_size = current->block_size - size;
                new_block->is_available = 1;
                new_block->data_ptr = (char*)current->data_ptr + size;
                new_block->next_block = current->next_block;

                // Update the current block
                current->block_size = size;
                current->is_available = 0;
                current->next_block = new_block;
            } else {
                current->is_available = 0;
            }

            return current->data_ptr;
        }
        current = current->next_block;
    }

    return NULL; // No suitable block found
}

// Free a previously allocated block within the pool
// Parameters:
// - ptr: pointer to the memory block to free
// Error handling:
// - Ignores NULL pointers and warns if the block is already free or not found.
void mem_free(void* ptr) {
    if (!ptr) {
        fprintf(stderr, "Warning: Attempted to free NULL pointer.\n");
        return;
    }

    MemBlock* current = pool_head;
    while (current != NULL) {
        if (current->data_ptr == ptr) {
            if (current->is_available) {
                fprintf(stderr, "Warning: Block at %p is already free.\n", ptr);
                return;
            }

            current->is_available = 1;

            // Merge adjacent free blocks to avoid fragmentation
            MemBlock* next_block = current->next_block;
            while (next_block != NULL && next_block->is_available) {
                current->block_size += next_block->block_size;
                current->next_block = next_block->next_block;
                free(next_block);
                next_block = current->next_block;
            }

            return;
        }
        current = current->next_block;
    }

    fprintf(stderr, "Warning: Pointer %p was not allocated from this pool.\n", ptr);
}

// Resize an allocated block within the pool
// Parameters:
// - ptr: pointer to the memory block to resize
// - size: the new size to allocate
// Returns:
// - Pointer to resized memory block or NULL if resizing fails
void* mem_resize(void* ptr, size_t size) {
    if (!ptr) return mem_alloc(size);  // Allocate new memory if NULL

    MemBlock* block = pool_head;
    while (block != NULL) {
        if (block->data_ptr == ptr) {
            if (block->block_size >= size) {
                return ptr; // No need to resize, current block is already big enough
            } else {
                void* new_ptr = mem_alloc(size);
                if (new_ptr) {
                    memcpy(new_ptr, ptr, block->block_size);
                    mem_free(ptr);
                }
                return new_ptr;
            }
        }
        block = block->next_block;
    }

    fprintf(stderr, "Warning: Resizing failed, pointer %p not found.\n", ptr);
    return NULL;
}

// Deinitialize the memory pool and release all resources
void mem_deinit() {
    free(pool_start);
    pool_start = NULL;

    MemBlock* current = pool_head;
    while (current != NULL) {
        MemBlock* next = current->next_block;
        free(current);
        current = next;
    }

    pool_head = NULL;
    total_pool_size = 0;
}
