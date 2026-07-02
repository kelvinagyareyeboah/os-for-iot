#include "drivers/uart.h"
#include "drivers/gpio.h"
#include "mmio.h"

// PL011 UART registers
#define UART0_DR (PBASE + 0x201000)
#define UART0_FR (PBASE + 0x201018)
#define UART0_IBRD (PBASE + 0x201024)
#define UART0_FBRD (PBASE + 0x201028)
#define UART0_LCRH (PBASE + 0x20102c)
#define UART0_CR (PBASE + 0x201030)
#define UART0_IMSC (PBASE + 0x201038)
#define UART0_ICR (PBASE + 0x201044)

void uart_init(void)
{
    // Enable GPIO 14 & 15 for UART0 (Alt0)
    gpio_use_as_alt0(14);
    gpio_use_as_alt0(15);

    // Disable UART0
    mmio_write(UART0_CR, 0x00000000);

    // Clear pending interrupts
    mmio_write(UART0_ICR, 0x7FF);

    // Set baud rate (Not strictly needed for QEMU but good practice)
    // Divider = 3000000 / (16 * 115200) = 1.627
    // Integer part = 1
    // Fractional part = 0.627 * 64 = 40
    mmio_write(UART0_IBRD, 1);
    mmio_write(UART0_FBRD, 40);

    // Enable FIFO & 8 bit data transmission (1 stop bit, no parity)
    mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

    // Enable UART0, get interrupts
    mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
                               (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));

    // Enable UART0, receive & transfer part of UART
    mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

void uart_send(char c)
{
    // Wait for UART to become ready to transmit
    while (mmio_read(UART0_FR) & (1 << 5))
    {
    }
    mmio_write(UART0_DR, c);
}

char uart_getc(void)
{
    // Wait for UART to have received something
    while (mmio_read(UART0_FR) & (1 << 4))
    {
    }
    return mmio_read(UART0_DR);
}

void uart_puts(const char *s)
{
    while (*s)
    {
        if (*s == '\n')
            uart_send('\r');
        uart_send(*s++);
    }
}
