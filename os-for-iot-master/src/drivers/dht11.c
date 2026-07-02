#include "../include/drivers/dht11.h"
#include "../include/drivers/gpio.h"
#include "../include/drivers/timer.h"

// Microsecond delay wrapper
void dl(int us)
{
    unsigned long start = get_system_timer();
    while ((get_system_timer() - start) < (unsigned long)us)
        ;
}

void dht11_init()
{
    // Usually nothing to init, just GPIO setup during read
}

dht11_result_t dht11_read()
{
    dht11_result_t res;
    res.success = 0;
    res.temperature = 0;
    res.humidity = 0;

    int pulseCounts[82];
    for (int i = 0; i < 82; i++)
        pulseCounts[i] = 0;

    // 1. Send Start Signal
    gpio_init_output(DHT_PIN);
    gpio_clear(DHT_PIN);
    dl(18000); // 18ms low
    gpio_set(DHT_PIN);
    dl(40);

    // 2. Switch to Input to listen
    gpio_use_as_alt0(DHT_PIN); // Not ideal, really need input mode.
    // NOTE: In our gpio.c, we don't have a pure "input" function helper.
    // 'gpio_use_as_alt0' sets it to Alt function 0 (which might be I2C or something else depending on pin).
    // To make this work properly, we need a gpio_init_input in gpio.c!
    // But for now, we will hack it or assume the user has updated gpio.c or use a trick.
    // Actually, setting FSEL to 000 is input.

    // Manually setting Input Mode since helper is missing
    unsigned int reg = GPFSEL0 + ((DHT_PIN / 10) * 4);
    unsigned int shift = (DHT_PIN % 10) * 3;
    unsigned int val = mmio_read(reg);
    val &= ~(7 << shift); // 000 is Input
    mmio_write(reg, val);

    // 3. Wait for response
    // Expect Low (80us) then High (80us)
    int timeout = 10000;
    while ((mmio_read(GPLEV0) & (1 << DHT_PIN)) == (1 << DHT_PIN))
    { // Wait for low
        if (--timeout == 0)
            return res;
    }

    // Now line is low. Wait for high.
    timeout = 10000;
    while ((mmio_read(GPLEV0) & (1 << DHT_PIN)) == 0)
    {
        if (--timeout == 0)
            return res;
    }

    // Now line is high. Wait for low.
    timeout = 10000;
    while ((mmio_read(GPLEV0) & (1 << DHT_PIN)) == (1 << DHT_PIN))
    {
        if (--timeout == 0)
            return res;
    }

    // 4. Read Data (40 bits)
    for (int i = 0; i < 40; i++)
    {
        // Wait for Low to end (Start of bit)
        timeout = 10000;
        while ((mmio_read(GPLEV0) & (1 << DHT_PIN)) == 0)
        {
            if (--timeout == 0)
                return res;
        }

        // Measure Length of High
        unsigned long tStart = get_system_timer();
        timeout = 10000;
        while ((mmio_read(GPLEV0) & (1 << DHT_PIN)) == (1 << DHT_PIN))
        {
            if (--timeout == 0)
                return res;
        }
        unsigned long tDur = get_system_timer() - tStart;

        // If > 50us (usually 70us for '1', 26-28us for '0'), it's a 1
        if (tDur > 50)
        {
            // Shift info
            if (i < 8)
                res.humidity |= (1 << (7 - i));
            else if (i < 16)
            { /* decimal part, ignore */
            }
            else if (i < 24)
                res.temperature |= (1 << (23 - i));
        }
    }

    res.success = 1;

    // Reset pin to high (Idle)
    gpio_init_output(DHT_PIN);
    gpio_set(DHT_PIN);

    return res;
}
