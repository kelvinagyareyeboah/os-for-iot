#include "../include/drivers/timer.h"
#include "../include/mmio.h"
#include "../include/drivers/uart.h"

// System Timer Registers
#define TIMER_CS (PBASE + 0x3000)
#define TIMER_CLO (PBASE + 0x3004)
#define TIMER_CHI (PBASE + 0x3008)
#define TIMER_C0 (PBASE + 0x300C)
#define TIMER_C1 (PBASE + 0x3010)
#define TIMER_C2 (PBASE + 0x3014)
#define TIMER_C3 (PBASE + 0x3018)

#define TIMER_CS_M0 (1 << 0)
#define TIMER_CS_M1 (1 << 1)
#define TIMER_CS_M2 (1 << 2)
#define TIMER_CS_M3 (1 << 3)

// Interval for timer interrupt (approx 1 second?)
// The clock runs at 1MHz (1 tick = 1us)
#define TIMER_INTERVAL 2000000

static unsigned int curVal = 0;

void timer_init(void)
{
    curVal = mmio_read(TIMER_CLO);
    curVal += TIMER_INTERVAL;
    mmio_write(TIMER_C1, curVal);
}

void handle_timer_irq(void)
{
    // Update comparator
    curVal += TIMER_INTERVAL;
    mmio_write(TIMER_C1, curVal);

    // Clear interrupt status (Write 1 to clear)
    mmio_write(TIMER_CS, TIMER_CS_M1);

    // Notify
    uart_puts("   [Timer Interrupt] Wake up! Collecting Sensor Data...\n");
}

unsigned long get_system_timer(void)
{
    unsigned int h = -1, l;
    h = mmio_read(TIMER_CHI);
    l = mmio_read(TIMER_CLO);
    if (h != mmio_read(TIMER_CHI))
    {
        h = mmio_read(TIMER_CHI);
        l = mmio_read(TIMER_CLO);
    }
    return (((unsigned long long)h) << 32) | l;
}
