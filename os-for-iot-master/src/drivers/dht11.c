

        // If > 50us (usually 70us for '1', 26-28us for '0'), it's a 1
                    // Shift info
                else if (i < 16)
        
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
