#include "allocator.h"
#include "buddy_allocator.h"
#include <stdio.h>
#include <time.h>

#define MEMORY_SIZE (1 << MAX_BUDDY_ORDER)
char global_memory[MEMORY_SIZE];

typedef struct Object {
    int id;
    char name[50];
    float value;
} Object;

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
