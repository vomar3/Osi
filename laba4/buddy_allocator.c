#include "buddy_allocator.h"
#include <math.h>

BuddyAllocator* buddy_allocator_create(void* memory, size_t size) {
    if (size < (1 << MAX_BUDDY_ORDER)) return NULL;

    BuddyAllocator* allocator = (BuddyAllocator*)memory;
    allocator->memory_start = (char*)memory + sizeof(BuddyAllocator);
    allocator->memory_size = size - sizeof(BuddyAllocator);

    for (int i = 0; i <= MAX_BUDDY_ORDER; i++) {
        allocator->free_lists[i] = NULL;
    }

    size_t initial_order = (size_t)log2(size);
    allocator->free_lists[initial_order] = (FreeBlock*)allocator->memory_start;
    allocator->free_lists[initial_order]->size = size;
    allocator->free_lists[initial_order]->next = NULL;

    return allocator;
}

void* buddy_allocator_alloc(BuddyAllocator* allocator, size_t size) {
    if (size == 0) return NULL;

    size_t order = (size_t)ceil(log2(size + sizeof(FreeBlock)));

    for (size_t i = order; i <= MAX_BUDDY_ORDER; i++) {
        if (allocator->free_lists[i]) {
            FreeBlock* block = allocator->free_lists[i];
            allocator->free_lists[i] = block->next;

            while (i > order) {
                i--;
                size_t block_size = 1 << i;
                FreeBlock* buddy = (FreeBlock*)((char*)block + block_size);
                buddy->size = block_size;
                buddy->next = allocator->free_lists[i];
                allocator->free_lists[i] = buddy;
            }

            block->size = (1 << order);
            return (char*)block + sizeof(FreeBlock);
        }
    }

    return NULL;
}

void buddy_allocator_free(BuddyAllocator* allocator, void* memory) {
    if (!memory) return;

    FreeBlock* block = (FreeBlock*)((char*)memory - sizeof(FreeBlock));
    size_t order = (size_t)log2(block->size);

    FreeBlock** current_list = &allocator->free_lists[order];
    block->next = *current_list;
    *current_list = block;
}
