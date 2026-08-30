
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
