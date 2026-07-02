#include "../include/drivers/framebuffer.h"
#include "../include/drivers/mailbox.h"

unsigned int width, height, pitch, isrgb;
unsigned char *fb_ptr;

// VERY simple 8x8 font bitmap for A-Z, 0-9, and basics
// To save space, this is a minimal font representation.
// Ideally, you'd include a full `font.h` here.
// Each byte represents a row of pixels.
const unsigned char font[128][8] = {
    [0 ... 127] = {0, 0, 0, 0, 0, 0, 0, 0}, // Default space
    ['0'] = {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00},
    ['1'] = {0x10, 0x30, 0x50, 0x10, 0x10, 0x10, 0x10, 0x00},
    ['2'] = {0x3C, 0x42, 0x02, 0x0C, 0x30, 0x40, 0x7E, 0x00},
    ['3'] = {0x3C, 0x42, 0x02, 0x1C, 0x02, 0x42, 0x3C, 0x00},
    ['4'] = {0x08, 0x18, 0x28, 0x48, 0x7E, 0x08, 0x08, 0x00},
    ['5'] = {0x7E, 0x40, 0x7C, 0x02, 0x02, 0x42, 0x3C, 0x00},
    ['6'] = {0x3C, 0x40, 0x7C, 0x42, 0x42, 0x42, 0x3C, 0x00},
    ['7'] = {0x7E, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x00},
    ['8'] = {0x3C, 0x42, 0x42, 0x3C, 0x42, 0x42, 0x3C, 0x00},
    ['9'] = {0x3C, 0x42, 0x42, 0x3E, 0x02, 0x02, 0x3C, 0x00},
    ['C'] = {0x3C, 0x42, 0x40, 0x40, 0x40, 0x42, 0x3C, 0x00},
    ['T'] = {0x7F, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00},
    ['e'] = {0x00, 0x3C, 0x42, 0x7E, 0x40, 0x42, 0x3C, 0x00},
    ['m'] = {0x00, 0x76, 0x49, 0x49, 0x49, 0x49, 0x49, 0x00},
    ['p'] = {0x00, 0x78, 0x44, 0x44, 0x78, 0x40, 0x40, 0x00},
    [':'] = {0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00},
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};

void fb_init()
{
    mbox[0] = 35 * 4;
    mbox[1] = 0; // Request

    mbox[2] = 0x48003; // set phy wh
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] = 1920;
    mbox[6] = 1080;

    mbox[7] = 0x48004; // set virt wh
    mbox[8] = 8;
    mbox[9] = 8;
    mbox[10] = 1920;
    mbox[11] = 1080;

    mbox[12] = 0x48009; // set virt offset
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] = 0;
    mbox[16] = 0;

    mbox[17] = 0x48005; // set depth
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = 32;

    mbox[21] = 0x48006; // set pixel order
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 1;

    mbox[25] = 0x40001; // get framebuffer
    mbox[26] = 8;
    mbox[27] = 8;
    mbox[28] = 4096;
    mbox[29] = 0;

    mbox[30] = 0x40008; // get pitch
    mbox[31] = 4;
    mbox[32] = 4;
    mbox[33] = 0;

    mbox[34] = 0; // End Tag

    if (mailbox_call(MBOX_CH_PROP) && mbox[20] == 32 && mbox[28] != 0)
    {
        width = mbox[5];
        height = mbox[6];
        pitch = mbox[33];
        isrgb = mbox[24];
        fb_ptr = (void *)((unsigned long)mbox[28] & 0x3FFFFFFF); // Convert to bus address
    }
}

void draw_char(int x, int y, char c)
{
    if (!fb_ptr)
        return;

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            // Calculate offset in buffer
            // each pixel is 4 bytes (32 bit)
            int offset = (y + row) * pitch + (x + col) * 4;

            if (font[(int)c][row] & (1 << (7 - col)))
            {
                // White pixel
                fb_ptr[offset] = 0xFF;     // Blue
                fb_ptr[offset + 1] = 0xFF; // Green
                fb_ptr[offset + 2] = 0xFF; // Red
                fb_ptr[offset + 3] = 0xFF; // Alpha
            }
            else
            {
                // Black background
                fb_ptr[offset] = 0x00;
                fb_ptr[offset + 1] = 0x00;
                fb_ptr[offset + 2] = 0x00;
                fb_ptr[offset + 3] = 0xFF;
            }
        }
    }
}

void fb_print(int x, int y, char *s)
{
    while (*s)
    {
        draw_char(x, y, *s);
        x += 8;
        s++;
    }
}

void fb_clear()
{
    // Simple black fill - slow but works
    if (!fb_ptr)
        return;
    for (unsigned int i = 0; i < height; i++)
    {
        for (unsigned int j = 0; j < width; j++)
        {
            int offset = i * pitch + j * 4;
            fb_ptr[offset] = 0;
            fb_ptr[offset + 1] = 0;
            fb_ptr[offset + 2] = 0;
        }
    }
}
