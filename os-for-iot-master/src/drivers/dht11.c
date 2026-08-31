#include "../include/drivers/dht11.h"
#include "../include/drivers/gpio.h"
#include "../include/drivers/timer.h"

/*
 * DHT11 pin
 * The DHT_PIN value is defined in dht11.h
 */


/*
 * Simple microsecond delay
 */
void dl(int us)
{
    unsigned long start = get_system_timer();

    while ((get_system_timer() - start) < (unsigned long)us)
    {
        /* Wait */
    }
}


/*
 * Initialize DHT11 sensor
 *
 * The DHT11 does not require any special initialization.
 * The GPIO pin is configured when a reading is requested.
 */
void dht11_init(void)
{
    /* Nothing required here */
}


/*
 * Read temperature and humidity from DHT11
 */
dht11_result_t dht11_read(void)
{
    dht11_result_t res;

    /* Default result */
    res.success = 0;
    res.temperature = 0;
    res.humidity = 0;


    /*
     * --------------------------------------------------------
     * 1. Send DHT11 Start Signal
     * --------------------------------------------------------
     */

    /* Set pin as output */
    gpio_init_output(DHT_PIN);

    /* Pull the data line LOW */
    gpio_clear(DHT_PIN);

    /* DHT11 requires at least 18ms LOW */
    dl(18000);

    /* Release the line */
    gpio_set(DHT_PIN);

    /* Wait briefly before listening */
    dl(40);


    /*
     * --------------------------------------------------------
     * 2. Change GPIO Pin to Input
     * --------------------------------------------------------
     *
     * The current GPIO driver does not provide a dedicated
     * gpio_init_input() function, so configure the GPIO
     * register directly.
     */

    unsigned int reg = GPFSEL0 + ((DHT_PIN / 10) * 4);
    unsigned int shift = (DHT_PIN % 10) * 3;

    unsigned int value = mmio_read(reg);

    /* GPIO function 000 = Input */
    value &= ~(7 << shift);

    mmio_write(reg, value);


    /*
     * --------------------------------------------------------
     * 3. Wait for DHT11 Response
     * --------------------------------------------------------
     *
     * The sensor responds with:
     *
     * LOW  ~80us
     * HIGH ~80us
     * LOW  ~50us
     */

    unsigned int pin_mask = (1 << DHT_PIN);
    int timeout;


    /* Wait for the sensor to pull the line LOW */
    timeout = 10000;

    while ((mmio_read(GPLEV0) & pin_mask) != 0)
    {
        if (--timeout == 0)
            return res;
    }


    /* Wait for the sensor to pull the line HIGH */
    timeout = 10000;

    while ((mmio_read(GPLEV0) & pin_mask) == 0)
    {
        if (--timeout == 0)
            return res;
    }


    /* Wait for the sensor to pull the line LOW again */
    timeout = 10000;

    while ((mmio_read(GPLEV0) & pin_mask) != 0)
    {
        if (--timeout == 0)
            return res;
    }


    /*
     * --------------------------------------------------------
     * 4. Read 40 Data Bits
     * --------------------------------------------------------
     *
     * DHT11 sends:
     *
     * 8 bits  - Humidity integer
     * 8 bits  - Humidity decimal
     * 8 bits  - Temperature integer
     * 8 bits  - Temperature decimal
     * 8 bits  - Checksum
     */

    for (int i = 0; i < 40; i++)
    {
        /*
         * Wait for the beginning of the bit.
         * Each bit starts with a LOW signal.
         */
        timeout = 10000;

        while ((mmio_read(GPLEV0) & pin_mask) == 0)
        {
            if (--timeout == 0)
                return res;
        }


        /*
         * Measure how long the signal stays HIGH.
         */
        unsigned long start_time = get_system_timer();

        timeout = 10000;

        while ((mmio_read(GPLEV0) & pin_mask) != 0)
        {
            if (--timeout == 0)
                return res;
        }

        unsigned long duration = get_system_timer() - start_time;


        /*
         * A HIGH pulse longer than approximately 50us
         * represents a binary 1.
         *
         * Shorter pulse = 0.
         */
        if (duration > 50)
        {
            /*
             * Humidity integer byte
             */
            if (i < 8)
            {
                res.humidity |= (1 << (7 - i));
            }

            /*
             * Humidity decimal byte
             * DHT11 normally sends 0 here, so ignore it.
             */
            else if (i < 16)
            {
                /* Ignore decimal humidity */
            }

            /*
             * Temperature integer byte
             */
            else if (i < 24)
            {
                res.temperature |= (1 << (23 - i));
            }

            /*
             * Temperature decimal and checksum
             * are ignored in this simplified implementation.
             */
            else
            {
                /* Ignore remaining bits */
            }
        }
    }


    /*
     * --------------------------------------------------------
     * 5. Reading Successful
     * --------------------------------------------------------
     */
    res.success = 1;


    /*
     * --------------------------------------------------------
     * 6. Return GPIO to Idle State
     * --------------------------------------------------------
     *
     * DHT11 data line should normally remain HIGH when idle.
     */
    gpio_init_output(DHT_PIN);
    gpio_set(DHT_PIN);


    return res;
}
