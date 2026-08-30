
        schedule(); // Yield to scheduler
    }
}

// Task 2: MQTT Protocol Simulation with Security
void task_mqtt(void)
{
    while (1) // Infinite task loop
    {
        uart_puts("   [MQTT] Establishing secure connection (TLS handshake)...\n");
        for (volatile int i = 0; i < 5000000; i++); // Simulate network delay

        uart_puts("   [MQTT] Publishing sensor data to broker...\n");
        uart_puts("   [MQTT] Packet: {\"temp\":");
        // Output temperature as two digits
        uart_send('0' + (simulated_temp / 10)); // Tens digit
        uart_send('0' + (simulated_temp % 10)); // Ones digit
        uart_puts(", \"humidity\":");
        // Output humidity as two digits
        uart_send('0' + (simulated_humidity / 10)); // Tens digit
        uart_send('0' + (simulated_humidity % 10)); // Ones digit
        uart_puts("} (encrypted)\n");

        uart_puts("   [MQTT] Sync complete. Power optimized.\n");

        // Sync delay
        for (volatile int i = 0; i < 20000000; i++);
        schedule(); // Yield to scheduler
    }
}

// Task 3: CoAP Protocol Simulation
void task_coap(void)
{
    while (1) // Infinite task loop
    {
        uart_puts("   [CoAP] Sending lightweight request to server...\n");
        uart_puts("   [CoAP] GET /sensor/data (UDP, no encryption for demo)\n");
        uart_puts("   [CoAP] Response: ACK with data\n");

        // Delay
        for (volatile int i = 0; i < 15000000; i++);
        schedule(); // Yield to scheduler
    }
}

void kernel_main(void)
{
    // Initialize hardware and kernel subsystems
    uart_init();    // Initialize UART for serial output
    mem_init();     // Initialize memory management
    sched_init();   // Initialize task scheduler

    // Print system banner
    uart_puts("\n\n");
    uart_puts("=========================================\n");
    uart_puts("  IoT Operating System - Simulated Demo\n");
    uart_puts("  Features: Multitasking, Power Mgmt, IoT Protocols\n");
    uart_puts("  Optimized for Low Resources\n");
    uart_puts("=========================================\n");

    // Display initial memory usage
    uart_puts("   [MEM] Heap used: ");
    uint32_t mem_used = get_heap_used(); // Get heap usage in bytes
    // Display kilobytes (simplified - shows 0-9 KB range)
    uart_send('0' + (mem_used / 1000)); // Thousands digit
    uart_send('0' + ((mem_used % 1000) / 100)); // Hundreds digit
    uart_puts(" KB\n");

    // Create Tasks (MQTT and CoAP run as scheduled tasks)
    task_create(task_mqtt);
    task_create(task_coap);

    // Main loop: Run scheduler and periodically show stats
    int cycle = 0; // Schedule cycle counter
    int data_available_counter = 0; // Sensor data counter
    while (1) // Main kernel loop
    {
        // Run sensor task inline (instead of as separate scheduled task)
        if (data_available_counter < 10)
        {
            // Simulate sensor reading (same logic as task_sensor)
            simulated_temp = (simulated_temp + 1) % 40;
            if (simulated_temp < 20) simulated_temp = 20;
            simulated_humidity = (simulated_humidity + 2) % 100;

            // Format and output sensor data
            char buf[64];
            char temp_str[12], hum_str[12];
            itoa(simulated_temp, temp_str);
            itoa(simulated_humidity, hum_str);
            
            int idx = 0;
            const char* prefix = "[SENSOR] Temp: ";
            for (int i = 0; prefix[i]; ++i) buf[idx++] = prefix[i];
            for (int i = 0; temp_str[i]; ++i) buf[idx++] = temp_str[i];
            buf[idx++] = 'C';
            const char* mid = ", Humidity: ";
            for (int i = 0; mid[i]; ++i) buf[idx++] = mid[i];
            for (int i = 0; hum_str[i]; ++i) buf[idx++] = hum_str[i];
            buf[idx++] = '%';
            buf[idx++] = '\n';
            buf[idx] = '\0';
            uart_puts(buf);

            data_available_counter++;

            // Short delay for readability
            for (volatile int i = 0; i < 100000; i++);
        }
        else
        {
            // Power saving mode
            uart_puts("   [PWR] No new data. Entering low-power mode (WFI sim)...\n");

            // Shorter sleep in main loop (300ms vs 3s in task)
            unsigned long start = get_system_timer();
            while ((get_system_timer() - start) < 300000UL);

            uart_puts("   [PWR] Waking up from low-power mode...\n");
            data_available_counter = 0; // Reset counter
        }

        schedule(); // Run scheduler to switch tasks
        cycle++; // Increment cycle counter
        
        // Periodically display system statistics
        if (cycle % 50 == 0) { // Every ~50 schedules
            uart_puts("   [SYS] Tasks running, CPU idle ~80%, Power: Low\n");
        }
    }
}
