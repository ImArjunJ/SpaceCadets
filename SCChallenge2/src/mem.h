#ifndef MEM_H
#define MEM_H

#include <stddef.h>

#define MEMORY_POOL_SIZE (512 * 1024) // 512 KB

typedef struct MemoryBlock
{
    size_t size;
    struct MemoryBlock* next;
} MemoryBlock;

// Initializes the memory pool
void initialize_memory_pool(void);
void* mem_malloc(size_t size);
void mem_free(void* ptr);
void* mem_memcpy(void* dest, const void* src, size_t n);
void* mem_realloc(void* ptr, size_t size);

#endif // MEM_H