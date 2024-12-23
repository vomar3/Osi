#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

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

typedef struct Object {
    int id;
    char name[50];
    float value;
} Object;


Allocator* allocator_create(void* memory, size_t size);

void allocator_destroy(Allocator* allocator);

void* allocator_alloc(Allocator* allocator, size_t size);

void allocator_free(Allocator* allocator, void* memory);

BuddyAllocator* buddy_allocator_create(void* memory, size_t size);

void buddy_allocator_destroy(BuddyAllocator* allocator);

void* buddy_allocator_alloc(BuddyAllocator* allocator, size_t size);

void buddy_allocator_free(BuddyAllocator* allocator, void* memory);

void measure_time(void (*func)(), const char* message);

int main() {
    struct timespec start, end;

    printf("Тестирование аллокатора с обычным списком свободных блоков:\n");

    Allocator* list_allocator = allocator_create(global_memory, MEMORY_SIZE);

    clock_gettime(CLOCK_MONOTONIC, &start);
    Object* object1 = allocator_alloc(list_allocator, sizeof(Object));
    clock_gettime(CLOCK_MONOTONIC, &end);
    double alloc_time1 = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Выделен блок: %p, время: %.9f секунд\n", object1, alloc_time1);

    clock_gettime(CLOCK_MONOTONIC, &start);
    Object* object2 = allocator_alloc(list_allocator, sizeof(Object));
    clock_gettime(CLOCK_MONOTONIC, &end);
    double alloc_time2 = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Выделен блок: %p, время: %.9f секунд\n", object2, alloc_time2);

    clock_gettime(CLOCK_MONOTONIC, &start);
    Object* object3 = allocator_alloc(list_allocator, sizeof(Object));
    clock_gettime(CLOCK_MONOTONIC, &end);
    double alloc_time3 = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Выделен блок: %p, время: %.9f секунд\n", object3, alloc_time3);

    if (object1) {
        object1->id = 1;
        snprintf(object1->name, sizeof(object1->name), "Object 1");
        object1->value = 123.45;
    }

    if (object2) {
        object2->id = 2;
        snprintf(object2->name, sizeof(object2->name), "Object 2");
        object2->value = 678.90;
    }

    if (object3) {
        object3->id = 3;
        snprintf(object3->name, sizeof(object3->name), "Object 3");
        object3->value = 135.79;
    }

    clock_gettime(CLOCK_MONOTONIC, &start);
    allocator_free(list_allocator, object1);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double free_time1 = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Освобождён блок: %p (id=%d, name=%s, value=%.2f), время: %.9f секунд\n", object1, object1->id, object1->name, object1->value, free_time1);

    clock_gettime(CLOCK_MONOTONIC, &start);
    allocator_free(list_allocator, object2);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double free_time2 = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Освобождён блок: %p (id=%d, name=%s, value=%.2f), время: %.9f секунд\n", object2, object2->id, object2->name, object2->value, free_time2);

    clock_gettime(CLOCK_MONOTONIC, &start);
    allocator_free(list_allocator, object3);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double free_time3 = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Освобождён блок: %p (id=%d, name=%s, value=%.2f), время: %.9f секунд\n", object3, object3->id, object3->name, object3->value, free_time3);

    printf("\nТестирование аллокатора с алгоритмом двойников:\n");

    BuddyAllocator* buddy_allocator = buddy_allocator_create(global_memory, MEMORY_SIZE);

    clock_gettime(CLOCK_MONOTONIC, &start);
    Object* buddy_object1 = buddy_allocator_alloc(buddy_allocator, sizeof(Object));
    clock_gettime(CLOCK_MONOTONIC, &end);
    double buddy_alloc_time1 = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Выделен блок: %p, время: %.9f секунд\n", buddy_object1, buddy_alloc_time1);

    clock_gettime(CLOCK_MONOTONIC, &start);
    Object* buddy_object2 = buddy_allocator_alloc(buddy_allocator, sizeof(Object));
    clock_gettime(CLOCK_MONOTONIC, &end);
    double buddy_alloc_time2 = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Выделен блок: %p, время: %.9f секунд\n", buddy_object2, buddy_alloc_time2);

    clock_gettime(CLOCK_MONOTONIC, &start);
    Object* buddy_object3 = buddy_allocator_alloc(buddy_allocator, sizeof(Object));
    clock_gettime(CLOCK_MONOTONIC, &end);
    double buddy_alloc_time3 = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Выделен блок: %p, время: %.9f секунд\n", buddy_object3, buddy_alloc_time3);

    if (buddy_object1) {
        buddy_object1->id = 1;
        snprintf(buddy_object1->name, sizeof(buddy_object1->name), "Buddy Object 1");
        buddy_object1->value = 123.45;
    }

    if (buddy_object2) {
        buddy_object2->id = 2;
        snprintf(buddy_object2->name, sizeof(buddy_object2->name), "Buddy Object 2");
        buddy_object2->value = 678.90;
    }

    if (buddy_object3) {
        buddy_object3->id = 3;
        snprintf(buddy_object3->name, sizeof(buddy_object3->name), "Buddy Object 3");
        buddy_object3->value = 135.79;
    }

    clock_gettime(CLOCK_MONOTONIC, &start);
    buddy_allocator_free(buddy_allocator, buddy_object1);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double buddy_free_time1 = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Освобождён блок: %p (id=%d, name=%s, value=%.2f), время: %.9f секунд\n", buddy_object1, buddy_object1->id, buddy_object1->name, buddy_object1->value, buddy_free_time1);

    clock_gettime(CLOCK_MONOTONIC, &start);
    buddy_allocator_free(buddy_allocator, buddy_object2);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double buddy_free_time2 = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Освобождён блок: %p (id=%d, name=%s, value=%.2f), время: %.9f секунд\n", buddy_object2, buddy_object2->id, buddy_object2->name, buddy_object2->value, buddy_free_time2);

    clock_gettime(CLOCK_MONOTONIC, &start);
    buddy_allocator_free(buddy_allocator, buddy_object3);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double buddy_free_time3 = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Освобождён блок: %p (id=%d, name=%s, value=%.2f), время: %.9f секунд\n", buddy_object3, buddy_object3->id, buddy_object3->name, buddy_object3->value, buddy_free_time3);
    allocator_destroy(list_allocator);

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

void allocator_destroy(Allocator* allocator) {}

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