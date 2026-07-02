import sys
import time
import re

print("IoT OS Text Monitor (Reading from STDIN)")

temp_history = []
humidity_history = []

for line in sys.stdin:
    line = line.strip()
    print(f"Log: {line}")
    
    # Parse temperature
    if "[SENSOR] Temp:" in line:
        match = re.search(r"Temp: (\d+)C, Humidity: (\d+)%", line)
        if match:
            temp = int(match.group(1))
            hum = int(match.group(2))
            temp_history.append(temp)
            humidity_history.append(hum)
            print(f"Parsed: Temp={temp}°C, Humidity={hum}%")
            if temp_history:
                print(f"Stats: Min Temp={min(temp_history)}, Max Temp={max(temp_history)}, Avg Temp={sum(temp_history)/len(temp_history):.1f}")
    
    # Parse MQTT
    if "[MQTT]" in line:
        print(f"MQTT Activity: {line}")
    
    # Parse CoAP
    if "[CoAP]" in line:
        print(f"CoAP Activity: {line}")
    
    # Parse system status
    if "[SYS]" in line:
        print(f"System: {line}")
    
    time.sleep(0.1)  # Small delay to avoid flooding