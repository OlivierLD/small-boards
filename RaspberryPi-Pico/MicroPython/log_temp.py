import machine
import utime

# Identify the sensor
temp_sensor = machine.ADC(4)    # machine.ADC_CORE_TEMP)


def read_temperature(sensor):
    conversion_factor = 3.3 / 65535
    reading = sensor.read_u16() * conversion_factor
    temperature = 27 - (reading - 0.706) / 0.001721
    return temperature


log_file = open("temperature.txt", "w")
keep_logging = True
while keep_logging:
    try:
        temperature = read_temperature(temp_sensor)
        print("Logging Temperature: {}".format(temperature))
        log_file.write(str(temperature) + "\n")
        log_file.flush()
        utime.sleep(2)
    except KeyboardInterrupt:
        print("\n\t\tUser interrupted, exiting.")
        keep_logging = False

log_file.close()
print("See file temperature.txt")
