#ifndef MEM_H
#define MEM_H

#include <stdint.h>

void mem_init(void);
void *kmalloc(uint32_t size);
void kfree(void *ptr); // Placeholder for now
uint32_t get_heap_used(void);

#endif
