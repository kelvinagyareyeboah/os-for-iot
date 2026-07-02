#include "drivers/gpio.h"

void gpio_use_as_alt5(unsigned int pin)
{
    unsigned int reg = GPFSEL0 + ((pin / 10) * 4);
    unsigned int shift = (pin % 10) * 3;
    unsigned int val = mmio_read(reg);
    val &= ~(7 << shift);
    val |= (2 << shift); // Alt5 is 2 (010 binary)
    mmio_write(reg, val);
}

void gpio_use_as_alt0(unsigned int pin)
{
    unsigned int reg = GPFSEL0 + ((pin / 10) * 4);
    unsigned int shift = (pin % 10) * 3;
    unsigned int val = mmio_read(reg);
    val &= ~(7 << shift);
    val |= (4 << shift); // Alt0 is 4 (100 binary)
    mmio_write(reg, val);
}

void gpio_init_output(unsigned int pin)
{
    unsigned int reg = GPFSEL0 + ((pin / 10) * 4);
    unsigned int shift = (pin % 10) * 3;
    unsigned int val = mmio_read(reg);
    val &= ~(7 << shift);
    val |= (1 << shift); // Output is 1 (001 binary)
    mmio_write(reg, val);
}

void gpio_set(unsigned int pin)
{
    if (pin < 32)
        mmio_write(GPSET0, 1 << pin);
    else
        mmio_write(GPSET1, 1 << (pin - 32));
}

void gpio_clear(unsigned int pin)
{
    if (pin < 32)
        mmio_write(GPCLR0, 1 << pin);
    else
        mmio_write(GPCLR1, 1 << (pin - 32));
}
