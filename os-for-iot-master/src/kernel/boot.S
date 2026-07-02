.section ".text.boot"
.global _start

// Vector Table (Must be at the start of binary)
_start:
    ldr pc, =reset
    ldr pc, =undefined_instruction
    ldr pc, =software_interrupt
    ldr pc, =prefetch_abort
    ldr pc, =data_abort
    ldr pc, =unused
    ldr pc, =irq
    ldr pc, =fiq

reset:
    // Move Vector Base Address Register (VBAR) to _start
    // Required for Cortex-A7 (RPi 2/3) to find our table
    ldr r0, =_start
    mcr p15, 0, r0, c12, c0, 0

    // Set up stack pointer
    ldr r0, =_start
    mov sp, r0

    // Clear BSS
    ldr r4, =__bss_start
    ldr r9, =__bss_end
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0
    b       2f
1:
    stmia r4!, {r5-r8}
2:
    cmp r4, r9
    blo 1b

    // Jump to main
    bl kernel_main

halt:
    wfe
    b halt

// Trap handlers
undefined_instruction: b .
software_interrupt: b .
prefetch_abort: b .
data_abort: b .
unused: b .
fiq: b .

// IRQ Handler Wrapper
irq:
    // 1. Correct return address (LR_irq has return address + 4)
    sub lr, lr, #4
    
    // 2. Save Context (R0-R12, LR) onto the stack
    push {r0-r12, lr}

    // 3. Call C handler
    bl irq_handler

    // 4. Restore Context
    pop {r0-r12, lr}

    // 5. Return from exception
    movs pc, lr

.global enable_irq
enable_irq:
    cpsie i
    bx lr
