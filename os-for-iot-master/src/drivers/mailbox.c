#include "../include/drivers/mailbox.h"

// Aligned to 16 bytes
volatile unsigned int __attribute__((aligned(16))) mbox[36];

int mailbox_call(unsigned char ch)
{
    unsigned int r = (((unsigned int)((unsigned long)&mbox) & ~0xF) | (ch & 0xF));

    // Wait until we can write to the mailbox
    while (mmio_read(MBOX_STATUS) & MBOX_FULL)
        ;

    // Write the address of our message to the mailbox
    mmio_write(MBOX_WRITE, r);

    // Wait for the response
    while (1)
    {
        // Wait until there is a message
        while (mmio_read(MBOX_STATUS) & MBOX_EMPTY)
            ;

        // Is it for us?
        if (r == mmio_read(MBOX_READ))
            return mbox[1] == 0x80000000; // Return true if success
    }
    return 0;
}
