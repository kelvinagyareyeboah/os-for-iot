#ifndef MAILBOX_H
#define MAILBOX_H

#include "../mmio.h"

// Mailbox Base Address
#define MBOX_BASE (PBASE + 0xB880)

#define MBOX_READ (MBOX_BASE + 0x00)
#define MBOX_STATUS (MBOX_BASE + 0x18)
#define MBOX_WRITE (MBOX_BASE + 0x20)

#define MBOX_FULL 0x80000000
#define MBOX_EMPTY 0x40000000

#define MBOX_CH_PROP 8

int mailbox_call(unsigned char ch);
extern volatile unsigned int mbox[36];

#endif
