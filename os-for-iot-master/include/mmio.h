#ifndef MMIO_H
#define MMIO_H

#include <stdint.h>

// Raspberry Pi 3/4 Physical Base Address for Peripherals
// RPi4 = 0xFE000000, RPi2/3 = 0x3F000000
// Changed to 0x3F000000 to support QEMU raspi2b/raspi3b simulation
#define PBASE 0x3F000000

static inline void mmio_write(uint32_t reg, uint32_t data)
{
    *(volatile uint32_t *)(reg) = data;
}

static inline uint32_t mmio_read(uint32_t reg)
{
    return *(volatile uint32_t *)(reg);
}

// Memory Barrier to ensure order
static inline void mmio_delay(int32_t count)
{
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
                 : "=r"(count) : [count] "0"(count) : "cc");
}

#endif
