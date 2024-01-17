#
# Log Temperature in a file
# From Raspberry Pi Pico.
#
import machine
import utime
"""
Read the Pico internal temperature sensor
Log data into a file.
"""

# Identify the sensor
temp_sensor = machine.ADC(4)    # machine.ADC_CORE_TEMP)

CONVERSION_FACTOR = 3.3 / 65535


def read_temperature(sensor: ADC) -> float:
    reading = sensor.read_u16() * CONVERSION_FACTOR
    temperature = 27 - (reading - 0.706) / 0.001721
    return temperature


log_file = open("temperature.txt", "w")
keep_logging = True
while keep_logging:
    try:
        temperature = read_temperature(temp_sensor)
        print("Logging Temperature: {}\272C".format(temperature))
        log_file.write(str(temperature) + "\n")
        log_file.flush()
        utime.sleep(2)
    except KeyboardInterrupt:
        print("\n\t\tUser interrupted, exiting.")
        keep_logging = False

log_file.close()
print("See file temperature.txt")
