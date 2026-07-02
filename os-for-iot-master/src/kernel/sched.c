#include "kernel/sched.h"
#include "drivers/uart.h"
#include <stdint.h>

struct task_struct tasks[MAX_TASKS];
struct task_struct *current_task;
int num_tasks = 0;

extern void cpu_switch_to(uint32_t **old_sp, uint32_t *new_sp);

void sched_init(void)
{
    // Task 0 is the init task (kernel_main)
    tasks[0].pid = 0;
    tasks[0].state = 2; // Running
    current_task = &tasks[0];
    num_tasks = 1;
}

void task_create(void (*func)(void))
{
    if (num_tasks >= MAX_TASKS)
        return;

    struct task_struct *t = &tasks[num_tasks];
    t->pid = num_tasks;
    t->state = 1; // Runnable

    // Prepare stack
    // Stack grows down. Top of stack is at stack + STACK_SIZE.
    uint32_t *stack_top = (uint32_t *)(t->stack + STACK_SIZE);

    // We need to emulate the stack content that cpu_switch_to expects to POP.
    // Order: r4, r5, r6, r7, r8, r9, r10, r11, lr
    // We push valid LR (func address) and dummy regs.

    stack_top -= 9; // Reserve space for 9 registers
    t->sp = stack_top;

    stack_top[8] = (uint32_t)func; // LR points to entry function

    num_tasks++;
}

void schedule(void)
{
    // Simple Round Robin
    int next_pid = (current_task->pid + 1);
    if (next_pid >= num_tasks)
        next_pid = 0;

    if (next_pid == current_task->pid)
        return; // No switch needed

    struct task_struct *next = &tasks[next_pid];
    struct task_struct *prev = current_task;

    current_task = next;

    // Debug: Show switch
    uart_puts("[SCHED] Switching to task ");
    uart_send('0' + next_pid);
    uart_puts("\n");

    // Switch context
    cpu_switch_to(&prev->sp, next->sp);
}
