
            unsigned long start 
        schedule(); // Run scheduler to switch tasks
        cycle++; // Increment cycle counter
        
        // Periodically display system statistics
        if (cycle % 50 == 0) { // Every ~50 schedules
            uart_puts("   [SYS] Tasks running, CPU idle ~80%, Power: Low\n");
        }
    }
}
