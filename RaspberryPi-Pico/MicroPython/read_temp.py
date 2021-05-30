import machine
import utime
"""
Read the Pico internal temperature sensor
Print data to stdout
"""

# Identify the sensor
temp_sensor = machine.ADC(4)    # machine.ADC_CORE_TEMP)

CONVERSION_FACTOR = 3.3 / 65535

def read_temperature(sensor: ADC) -> float:
    reading = sensor.read_u16() * CONVERSION_FACTOR
    temperature = 27 - (reading - 0.706) / 0.001721
    return temperature


while True:
    temperature = read_temperature(temp_sensor)
    print("Temperature: {}\272C".format(temperature))
    utime.sleep(2)
