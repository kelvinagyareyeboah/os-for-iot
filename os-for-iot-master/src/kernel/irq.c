#include "drivers/irq.h"
#include "mmio.h"
#include "drivers/uart.h"

// External handler for timer (defined in timer.c)
extern void handle_timer_irq(void);

void irq_init(void)
{
    mmio_write(IRQ_DISABLE_1, 0xFFFFFFFF);
    mmio_write(IRQ_DISABLE_2, 0xFFFFFFFF);
    mmio_write(IRQ_DISABLE_BASIC, 0xFFFFFFFF);
}

void enable_interrupt_controller(void)
{
    // Enable System Timer 1 (IRQ 1)
    mmio_write(IRQ_ENABLE_1, SYSTEM_TIMER_IRQ_1);
}

void irq_handler(void)
{
    unsigned int pending = mmio_read(IRQ_PENDING_1);

    if (pending & SYSTEM_TIMER_IRQ_1)
    {
        handle_timer_irq();
    }
    else
    {
        // Unknown interrupt
    }
}
