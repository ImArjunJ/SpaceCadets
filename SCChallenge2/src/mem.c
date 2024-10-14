#include "mem.h"

static char memory_pool[MEMORY_POOL_SIZE];

// Head of the free list
static MemoryBlock* free_list = NULL;

// Initializes the memory pool by setting up the free list
void initialize_memory_pool(void)
{
    free_list = (MemoryBlock*) memory_pool;
    free_list->size = MEMORY_POOL_SIZE - sizeof(MemoryBlock); // Leave space for the header
    free_list->next = NULL;                                   // End of the list
}

// Allocates memory of the given size from the memory pool
void* mem_malloc(size_t size)
{
    MemoryBlock* current = free_list;
    MemoryBlock* previous = NULL;

    // Align the size to be a multiple of 8 bytes for better memory alignment
    size = (size + 7) & ~7;

    while (current)
    {
        if (current->size >= size)
        {
            // Found a block that is big enough
            if (current->size > size + sizeof(MemoryBlock))
            {
                // Split the block
                MemoryBlock* new_block = (MemoryBlock*) ((char*) current + sizeof(MemoryBlock) + size);
                new_block->size = current->size - size - sizeof(MemoryBlock);
                new_block->next = current->next;

                if (previous)
                {
                    previous->next = new_block;
                }
                else
                {
                    free_list = new_block; // New head of the free list
                }
            }
            else
            {
                // Just use the whole block
                if (previous)
                {
                    previous->next = current->next; // Remove the block from the free list
                }
                else
                {
                    free_list = current->next; // Update the head of the free list
                }
            }
            current->size = size;                                   // Update the block size
            return (void*) ((char*) current + sizeof(MemoryBlock)); // Return a pointer to the allocated memory
        }
        previous = current;
        current = current->next; // Move to the next block
    }
    return NULL; // No suitable block found
}

// Frees the allocated memory and adds it back to the free list
void mem_free(void* ptr)
{
    if (!ptr)
        return; // Nothing to free

    MemoryBlock* block = (MemoryBlock*) ((char*) ptr - sizeof(MemoryBlock));
    block->next = free_list; // Add the block to the front of the free list
    free_list = block;
}

// Copies memory from source to destination
void* mem_memcpy(void* dest, const void* src, size_t n)
{
    // Simple implementation of memcpy
    char* d = (char*) dest;
    const char* s = (const char*) src;
    for (size_t i = 0; i < n; i++)
    {
        d[i] = s[i];
    }
    return dest;
}

// Reallocates memory to a new size
void* mem_realloc(void* ptr, size_t size)
{
    if (!ptr)
    {
        return mem_malloc(size);
    }

    if (size == 0)
    {
        mem_free(ptr);
        return NULL;
    }

    MemoryBlock* block = (MemoryBlock*) ((char*) ptr - sizeof(MemoryBlock));
    if (block->size >= size)
    {
        // Current block is already large enough
        // Optionally, we could split the block here
        return ptr;
    }
    else
    {
        // Allocate a new block
        void* new_ptr = mem_malloc(size);
        if (!new_ptr)
        {
            return NULL;
        }

        // Copy the data to the new block
        mem_memcpy(new_ptr, ptr, block->size < size ? block->size : size);

        // Free the old block
        mem_free(ptr);

        return new_ptr;
    }
}
