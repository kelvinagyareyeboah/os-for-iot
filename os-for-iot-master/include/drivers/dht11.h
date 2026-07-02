#ifndef DHT11_H
#define DHT11_H

// Using GPIO 4 for Data
#define DHT_PIN 4

typedef struct
{
    int temperature;
    int humidity;
    int success; // 1 if read verified, 0 if fail
} dht11_result_t;

void dht11_init(void);
dht11_result_t dht11_read(void);

#endif
