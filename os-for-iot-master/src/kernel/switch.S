.global cpu_switch_to
cpu_switch_to:
    // R0 = Pointer to location to save old SP (&current->sp)
    // R1 = New SP value (next->sp)

    // Save callee-saved registers of current task
    push {r4-r11, lr}

    // Save current Stack Pointer to the TCB
    str sp, [r0]

    // Load new Stack Pointer from the next TCB
    mov sp, r1

    // Restore callee-saved registers of new task
    pop {r4-r11, lr}

    // Return (into the new task)
    bx lr
