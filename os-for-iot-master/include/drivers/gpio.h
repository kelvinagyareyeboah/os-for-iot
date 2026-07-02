#ifndef GPIO_H
#define GPIO_H

#include "../mmio.h"

#define GPFSEL0 (PBASE + 0x200000)
#define GPFSEL1 (PBASE + 0x200004)
#define GPSET0 (PBASE + 0x20001C)
#define GPSET1 (PBASE + 0x200020)

#define GPCLR0 (PBASE + 0x200028)
#define GPCLR1 (PBASE + 0x20002C)

// RPi4 Pull-up/down registers
#define GPIO_PUP_PDN_CNTRL_REG0 (PBASE + 0x2000E4)

#define GPLEV0 (PBASE + 0x200034) // Pin Level

void gpio_use_as_alt5(unsigned int pin);
void gpio_use_as_alt0(unsigned int pin);
void gpio_init_output(unsigned int pin);
void gpio_set(unsigned int pin);
void gpio_clear(unsigned int pin);

#endif
