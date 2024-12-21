#include <stddef.h>
#include <stdio.h>
#include <math.h>

#define MAX_BUDDY_ORDER 20
#define MEMORY_SIZE (1 << MAX_BUDDY_ORDER)

char global_memory[MEMORY_SIZE];

typedef struct FreeBlock {
    size_t size;
    struct FreeBlock* next;
} FreeBlock;

typedef struct Allocator {
    void* memory_start;
    size_t memory_size;
    FreeBlock* free_list;
} Allocator;

typedef struct BuddyAllocator {
    void* memory_start;
    size_t memory_size;
    FreeBlock* free_lists[MAX_BUDDY_ORDER + 1];
} BuddyAllocator;

Allocator* allocator_create(void* memory, size_t size);

void allocator_destroy(Allocator* allocator);

void* allocator_alloc(Allocator* allocator, size_t size);

void allocator_free(Allocator* allocator, void* memory);

BuddyAllocator* buddy_allocator_create(void* memory, size_t size);

void buddy_allocator_destroy(BuddyAllocator* allocator);

void* buddy_allocator_alloc(BuddyAllocator* allocator, size_t size);

void buddy_allocator_free(BuddyAllocator* allocator, void* memory);

void test_allocators();

int main() {
    test_allocators();

    return 0;
}

Allocator* allocator_create(void* memory, size_t size) {
    if (size < sizeof(FreeBlock)) {
        return NULL;
    }

    Allocator* allocator = (Allocator*)memory;
    allocator->memory_start = (char*)memory + sizeof(Allocator);
    allocator->memory_size = size - sizeof(Allocator);
    allocator->free_list = (FreeBlock*)allocator->memory_start;

    allocator->free_list->size = allocator->memory_size;
    allocator->free_list->next = NULL;

    return allocator;
}

void allocator_destroy(Allocator* allocator) {
    // Ничего не делаем, так как память глобальная
}

void* allocator_alloc(Allocator* allocator, size_t size) {
    if (size == 0) {
        return NULL;
    }

    FreeBlock* prev = NULL;
    FreeBlock* current = allocator->free_list;

    while (current) {
        if (current->size >= size + sizeof(FreeBlock)) {
            if (current->size > size + sizeof(FreeBlock)) {
                FreeBlock* new_block = (FreeBlock*)((char*)current + sizeof(FreeBlock) + size);
                new_block->size = current->size - size - sizeof(FreeBlock);
                new_block->next = current->next;
                current->next = new_block;
            }

            if (prev) {
                prev->next = current->next;
            } else {
                allocator->free_list = current->next;
            }

            current->size = size;
            return (char*)current + sizeof(FreeBlock);
        }

        prev = current;
        current = current->next;
    }

    return NULL;
}

void allocator_free(Allocator* allocator, void* memory) {
    if (!memory) {
        return;
    }

    FreeBlock* block_to_free = (FreeBlock*)((char*)memory - sizeof(FreeBlock));
    block_to_free->next = allocator->free_list;
    allocator->free_list = block_to_free;
}

BuddyAllocator* buddy_allocator_create(void* memory, size_t size) {
    if (size < (1 << MAX_BUDDY_ORDER)) {
        return NULL;
    }

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

void buddy_allocator_destroy(BuddyAllocator* allocator) {
    // Ничего не делаем, так как память глобальная
}

void* buddy_allocator_alloc(BuddyAllocator* allocator, size_t size) {
    if (size == 0) {
        return NULL;
    }

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
    if (!memory) {
        return;
    }

    FreeBlock* block = (FreeBlock*)((char*)memory - sizeof(FreeBlock));
    size_t order = (size_t)log2(block->size);

    FreeBlock** current_list = &allocator->free_lists[order];
    block->next = *current_list;
    *current_list = block;
}

void test_allocators() {
    printf("Тестирование списка свободных блоков\n");

    Allocator* list_allocator = allocator_create(global_memory, MEMORY_SIZE);
    void* block1 = allocator_alloc(list_allocator, 256);
    printf("Выделен блок: %p\n", block1);

    void* block2 = allocator_alloc(list_allocator, 512);
    printf("Выделен блок: %p\n", block2);

    allocator_free(list_allocator, block1);
    printf("Освобождён блок: %p\n", block1);

    allocator_free(list_allocator, block2);
    printf("Освобождён блок: %p\n", block2);

    allocator_destroy(list_allocator);

    printf("\nТестирование алгоритма двойников\n");

    BuddyAllocator* buddy_allocator = buddy_allocator_create(global_memory, MEMORY_SIZE);
    void* buddy_block1 = buddy_allocator_alloc(buddy_allocator, 256);
    printf("Выделен блок: %p\n", buddy_block1);

    void* buddy_block2 = buddy_allocator_alloc(buddy_allocator, 512);
    printf("Выделен блок: %p\n", buddy_block2);

    buddy_allocator_free(buddy_allocator, buddy_block1);
    printf("Освобождён блок: %p\n", buddy_block1);

    buddy_allocator_free(buddy_allocator, buddy_block2);
    printf("Освобождён блок: %p\n", buddy_block2);

    buddy_allocator_destroy(buddy_allocator);
}