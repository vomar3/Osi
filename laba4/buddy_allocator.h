#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <stddef.h>

#define MAX_BUDDY_ORDER 20

typedef struct FreeBlock {
    size_t size;
    struct FreeBlock* next;
} FreeBlock;

typedef struct BuddyAllocator {
    void* memory_start;
    size_t memory_size;
    FreeBlock* free_lists[MAX_BUDDY_ORDER + 1];
} BuddyAllocator;

BuddyAllocator* buddy_allocator_create(void* memory, size_t size);
void buddy_allocator_destroy(BuddyAllocator* allocator);
void* buddy_allocator_alloc(BuddyAllocator* allocator, size_t size);
void buddy_allocator_free(BuddyAllocator* allocator, void* memory);

#endif // BUDDY_ALLOCATOR_H
