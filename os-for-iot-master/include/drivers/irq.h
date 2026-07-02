#ifndef IRQ_H
#define IRQ_H

#include "../mmio.h"

// IRQ Controller Registers (BCM2835)
#define IRQ_BASIC_PENDING (PBASE + 0xB200)
#define IRQ_PENDING_1 (PBASE + 0xB204)
#define IRQ_PENDING_2 (PBASE + 0xB208)
#define IRQ_FIQ_CONTROL (PBASE + 0xB20C)
#define IRQ_ENABLE_1 (PBASE + 0xB210)
#define IRQ_ENABLE_2 (PBASE + 0xB214)
#define IRQ_ENABLE_BASIC (PBASE + 0xB218)
#define IRQ_DISABLE_1 (PBASE + 0xB21C)
#define IRQ_DISABLE_2 (PBASE + 0xB220)
#define IRQ_DISABLE_BASIC (PBASE + 0xB224)

#define SYSTEM_TIMER_IRQ_1 (1 << 1)
#define SYSTEM_TIMER_IRQ_3 (1 << 3)

void irq_init(void);
void enable_interrupt_controller(void);
void irq_handler(void);

#endif
