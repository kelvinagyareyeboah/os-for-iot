#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>

#define MAX_TASKS 4
#define STACK_SIZE 2048

struct cpu_context
{
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t lr;
};

struct task_struct
{
    struct cpu_context context; // Must be top of stack for easy popped
    uint32_t pid;
    uint32_t state; // 0=Free, 1=Runnable, 2=Running
    uint8_t stack[STACK_SIZE];
    uint32_t *sp; // Saved Stack Pointer
};

void sched_init(void);
void task_create(void (*func)(void));
void schedule(void);
void cpu_switch_to(uint32_t **old_sp, uint32_t *new_sp);

#endif
