#include "../include/mm/mem.h"

extern uint8_t __bss_end; // Defined in linker.ld

static uint8_t *heap_start;
static uint8_t *heap_curr;

#define HEAP_MAX_SIZE 0x100000 // 1MB Heap

void mem_init(void)
{
    heap_start = &__bss_end;
    heap_curr = heap_start;
}

void *kmalloc(uint32_t size)
{
    // Algin to 4 bytes
    uint32_t remainder = size % 4;
    if (remainder != 0)
    {
        size += (4 - remainder);
    }

    if ((heap_curr + size) - heap_start > HEAP_MAX_SIZE)
    {
        return 0; // Out of memory
    }

    uint8_t *ptr = heap_curr;
    heap_curr += size;
    return (void *)ptr;
}

void kfree(void *ptr)
{
    // A simple bump allocator cannot easily free memory
    // without a more complex structure (linked list, bitmap).
    // For this simple IoT OS demo, we assume infinite heap
    // or reset functionality for now.
}

uint32_t get_heap_used(void)
{
    return (uint32_t)(heap_curr - heap_start);
}
