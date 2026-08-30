#include <stddef.h>
#include <stdint.h>

#include "drivers/uart.h"
#include "drivers/timer.h"
#include "drivers/irq.h"
#include "mm/mem.h"
#include "kernel/sched.h"

extern void enable_irq(void);

/*
 * ============================================================
 * Simple Integer to String Conversion
 * ============================================================
 *
 * Converts an integer into a null-terminated string.
 * No printf/sprintf dependencies are required.
 */
void itoa(int value, char *str)
{
    char buf[12];
    int i = 0;
    int j = 0;
    int negative = 0;

    /* Handle zero */
    if (value == 0)
    {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    /* Handle negative values */
    if (value < 0)
    {
        negative = 1;
        value = -value;
    }

    /* Extract digits in reverse order */
    while (value > 0)
    {
        buf[i++] = (char)((value % 10) + '0');
        value /= 10;
    }

    /* Add negative sign if necessary */
    if (negative)
    {
        str[j++] = '-';
    }

    /* Reverse digits */
    while (i > 0)
    {
        str[j++] = buf[--i];
    }

    str[j] = '\0';
}


/*
 * ============================================================
 * Simulated DHT11 Sensor Data
 * ============================================================
 */

int simulated_temp = 25;
int simulated_humidity = 50;


/*
 * ============================================================
 * Sensor Simulation
 * ============================================================
 *
 * Updates the simulated temperature and humidity values.
 */
static void update_sensor_data(void)
{
    /* Increase temperature and wrap around at 40 */
    simulated_temp = (simulated_temp + 1) % 40;

    /* Keep temperature at a reasonable minimum */
    if (simulated_temp < 20)
    {
        simulated_temp = 20;
    }

    /* Increase humidity and wrap around at 100 */
    simulated_humidity = (simulated_humidity + 2) % 100;
}


/*
 * ============================================================
 * Sensor Output
 * ============================================================
 *
 * Formats and prints the current sensor readings.
 */
static void print_sensor_data(void)
{
    char buf[64];
    char temp_str[12];
    char hum_str[12];

    int idx = 0;

    const char *prefix = "[SENSOR] Temp: ";
    const char *middle = ", Humidity: ";

    /* Convert values to strings */
    itoa(simulated_temp, temp_str);
    itoa(simulated_humidity, hum_str);

    /* Copy prefix */
    for (int i = 0; prefix[i] != '\0'; ++i)
    {
        buf[idx++] = prefix[i];
    }

    /* Copy temperature */
    for (int i = 0; temp_str[i] != '\0'; ++i)
    {
        buf[idx++] = temp_str[i];
    }

    /* Temperature unit */
    buf[idx++] = 'C';

    /* Copy middle text */
    for (int i = 0; middle[i] != '\0'; ++i)
    {
        buf[idx++] = middle[i];
    }

    /* Copy humidity */
    for (int i = 0; hum_str[i] != '\0'; ++i)
    {
        buf[idx++] = hum_str[i];
    }

    /* Humidity unit */
    buf[idx++] = '%';

    /* New line */
    buf[idx++] = '\n';

    /* Null terminator */
    buf[idx] = '\0';

    uart_puts(buf);
}


/*
 * ============================================================
 * Sensor Threshold Processing
 * ============================================================
 */
static void process_sensor_data(void)
{
    if (simulated_temp > 30)
    {
        uart_puts("   [ALERT] High temperature detected!\n");
    }
}


/*
 * ============================================================
 * Task 1: Sensor Data Collection and Processing
 * ============================================================
 */
void task_sensor(void)
{
    uart_puts("SENSOR RUNNING\n");

    int data_available_counter = 0;

    while (1)
    {
        /*
         * Collect up to 10 sensor readings before
         * entering simulated low-power mode.
         */
        if (data_available_counter < 10)
        {
            /* Simulate sensor reading */
            update_sensor_data();

            /* Display sensor data */
            print_sensor_data();

            /* Confirm task execution */
            uart_puts("[DEBUG] Sensor task executed\n");

            /* Check sensor thresholds */
            process_sensor_data();

            data_available_counter++;

            /*
             * Delay for readability.
             * volatile prevents the compiler from removing
             * the busy-wait loop.
             */
            for (volatile int i = 0; i < 10000000; i++)
            {
            }
        }
        else
        {
            /*
             * Simulated power-saving mode.
             */
            uart_puts(
                "   [PWR] No new data. "
                "Entering low-power mode (WFI sim)...\n"
            );

            unsigned long start = get_system_timer();

            /*
             * Simulate approximately 3 seconds of low-power
             * waiting using the system timer.
             */
            while ((get_system_timer() - start) < 3000000UL)
            {
            }

            uart_puts(
                "   [PWR] Waking up from low-power mode...\n"
            );

            data_available_counter = 0;
        }

        /*
         * Give the scheduler an opportunity to run
         * another task.
         */
        schedule();
    }
}


/*
 * ============================================================
 * Task 2: MQTT Protocol Simulation
 * ============================================================
 *
 * Simulates a secure MQTT connection and sensor
 * data publication.
 */
void task_mqtt(void)
{
    while (1)
    {
        uart_puts(
            "   [MQTT] Establishing secure connection "
            "(TLS handshake)...\n"
        );

        /* Simulated network/TLS delay */
        for (volatile int i = 0; i < 5000000; i++)
        {
        }

        uart_puts(
            "   [MQTT] Publishing sensor data to broker...\n"
        );

        /*
         * Display simulated JSON packet.
         */
        uart_puts("   [MQTT] Packet: {\"temp\":");

        /* Temperature - two digits */
        uart_send((char)('0' + (simulated_temp / 10)));
        uart_send((char)('0' + (simulated_temp % 10)));

        uart_puts(", \"humidity\":");

        /* Humidity - two digits */
        uart_send((char)('0' + (simulated_humidity / 10)));
        uart_send((char)('0' + (simulated_humidity % 10)));

        uart_puts("} (encrypted)\n");

        uart_puts(
            "   [MQTT] Sync complete. Power optimized.\n"
        );

        /* Simulated synchronization delay */
        for (volatile int i = 0; i < 20000000; i++)
        {
        }

        /* Yield to scheduler */
        schedule();
    }
}


/*
 * ============================================================
 * Task 3: CoAP Protocol Simulation
 * ============================================================
 */
void task_coap(void)
{
    while (1)
    {
        uart_puts(
            "   [CoAP] Sending lightweight request to server...\n"
        );

        uart_puts(
            "   [CoAP] GET /sensor/data "
            "(UDP, no encryption for demo)\n"
        );

        uart_puts(
            "   [CoAP] Response: ACK with data\n"
        );

        /* Simulated network delay */
        for (volatile int i = 0; i < 15000000; i++)
        {
        }

        /* Yield to scheduler */
        schedule();
    }
}


/*
 * ============================================================
 * Memory Usage Display
 * ============================================================
 *
 * Displays the current heap usage without relying on printf.
 */
static void print_memory_usage(void)
{
    uint32_t mem_used = get_heap_used();

    uart_puts("   [MEM] Heap used: ");

    /*
     * Display the value in KB.
     *
     * This supports values larger than the original
     * two-character display.
     */
    char mem_str[16];

    uint32_t mem_kb = mem_used / 1000;

    itoa((int)mem_kb, mem_str);

    uart_puts(mem_str);
    uart_puts(" KB\n");
}


/*
 * ============================================================
 * Kernel Main
 * ============================================================
 */
void kernel_main(void)
{
    /*
     * --------------------------------------------------------
     * Initialize hardware and kernel subsystems
     * --------------------------------------------------------
     */
    uart_init();
    mem_init();
    sched_init();

    /*
     * --------------------------------------------------------
     * System Banner
     * --------------------------------------------------------
     */
    uart_puts("\n\n");

    uart_puts("=========================================\n");
    uart_puts("  IoT Operating System - Simulated Demo\n");
    uart_puts("  Features: Multitasking, Power Mgmt, IoT Protocols\n");
    uart_puts("  Optimized for Low Resources\n");
    uart_puts("=========================================\n");

    /*
     * --------------------------------------------------------
     * Display Memory Usage
     * --------------------------------------------------------
     */
    print_memory_usage();

    /*
     * --------------------------------------------------------
     * Create Kernel Tasks
     * --------------------------------------------------------
     *
     * MQTT and CoAP are scheduled as independent tasks.
     */
    task_create(task_mqtt);
    task_create(task_coap);

    /*
     * --------------------------------------------------------
     * Main Kernel Loop
     * --------------------------------------------------------
     *
     * Sensor processing remains inline in the kernel loop,
     * matching the original implementation.
     */
    int cycle = 0;
    int data_available_counter = 0;

    while (1)
    {
        /*
         * Collect 10 sensor readings before entering
         * simulated power-saving mode.
         */
        if (data_available_counter < 10)
        {
            /* Update simulated sensor values */
            update_sensor_data();

            /* Display sensor readings */
            print_sensor_data();

            data_available_counter++;

            /*
             * Short delay for readability.
             */
            for (volatile int i = 0; i < 100000; i++)
            {
            }
        }
        else
        {
            /*
             * ------------------------------------------------
             * Simulated Low-Power Mode
             * ------------------------------------------------
             */
            uart_puts(
                "   [PWR] No new data. "
                "Entering low-power mode (WFI sim)...\n"
            );

            unsigned long start = get_system_timer();

            /*
             * Simulate approximately 300 ms of sleeping.
             */
            while ((get_system_timer() - start) < 300000UL)
            {
            }

            uart_puts(
                "   [PWR] Waking up from low-power mode...\n"
            );

            data_available_counter = 0;
        }

        /*
         * ------------------------------------------------
         * Run Scheduler
         * ------------------------------------------------
         */
        schedule();

        cycle++;

        /*
         * ------------------------------------------------
         * Periodic System Statistics
         * ------------------------------------------------
         */
        if (cycle % 50 == 0)
        {
            uart_puts(
                "   [SYS] Tasks running, CPU idle ~80%, "
                "Power: Low\n"
            );
        }
    }
}
