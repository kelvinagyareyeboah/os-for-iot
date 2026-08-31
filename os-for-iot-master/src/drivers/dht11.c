t function 0 (which might be I2C or something else depending on pin).
    // To make this work properly, we need a gpio_init_input in gpio.c!
    // But for now, we will hacr has updated gpio.c or use a trick.
    // Actually, setting FSEL t    // Manually setting Input Mode since helper is missing
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
