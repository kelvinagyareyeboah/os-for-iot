# Deployment Guide: From Simulation to Real Hardware

## 1. Hardware Required
To run this IoT OS wirelessly and see the data on a laptop, you need:
1.  **Raspberry Pi 4 Model B**.
2.  **MicroSD Card** (8GB+).
3.  **USB-to-TTL Serial Cable** (e.g., CP2102 or FTDI) - *For wired debugging*.
4.  **ESP8266 WiFi Module** (e.g., ESP-01) - *For Wireless Connectivity*.
    *   *Note: Writing a bare-metal driver for the Pi 4's internal WiFi is extremely difficult. Standard IoT OSs use an external module like ESP8266 connected via UART to handle WiFi.*

## 2. Boot Settings (config.txt)
Create a `config.txt` on the SD card:
```text
kernel=kernel7.img
enable_uart=1
dtoverlay=disable-bt
core_freq=250
```

## 3. Code Changes for Raspberry Pi 4
In `include/mmio.h`:
```c
// Change this line for Real Hardware
#define PBASE 0xFE000000 
```

## 4. Wireless Connection (The ESP8266 Approach)
The OS interacts with the ESP8266 using "AT Commands" over UART.
1.  **Connect ESP8266 to Pi:**
    *   ESP TX -> Pi RX (Pin 10 / GPIO 15)
    *   ESP RX -> Pi TX (Pin 8 / GPIO 14)
    *   VCC -> 3.3V
    *   GND -> GND
2.  **OS Logic:**
    The OS sends: `AT+CIPSTART="TCP","192.168.1.100",1883`
    Then sends data: `AT+CIPSEND...`

## 5. Laptop GUI (The Receiver)
We have provided a Python script `tools/monitor.py` that runs on your laptop.
1.  It connects to the Serial Port (or listens on a socket).
2.  It parses the lines looking for `[T1] Sensor Reading: X`.
3.  It updates a graphical dashboard.
