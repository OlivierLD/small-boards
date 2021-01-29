import machine
import utime

# Identify the sensor
temp_sensor = machine.ADC(4)    # machine.ADC_CORE_TEMP)


def read_temperature(sensor):
    conversion_factor = 3.3 / 65535
    reading = sensor.read_u16() * conversion_factor
    temperature = 27 - (reading - 0.706) / 0.001721
    return temperature


while True:
    temperature = read_temperature(temp_sensor)
    print("Temperature: {}".format(temperature))
    utime.sleep(2)
