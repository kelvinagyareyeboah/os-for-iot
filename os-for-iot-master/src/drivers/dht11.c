LEV0) & (1 

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
