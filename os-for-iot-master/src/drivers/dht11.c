
        // If > 50us (usually 70us for '1', 26-28us f
                    /
                else if (i < 16                res.temperature |= (1 << (23 -     res.success = 1;

    // Reset pin to high (Idle)
    gpio_init_output(DHT_PIN);
    gpio_set(DHT_PIN);

    return res;
}
