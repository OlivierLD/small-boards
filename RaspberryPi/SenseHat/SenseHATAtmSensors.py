#!/usr/bin/python
# import sys
from sense_hat import SenseHat

sense = SenseHat()

pressure = sense.get_pressure()
print("Pressure: %s Millibars" % pressure)

# alternatives
print("Alt: Pressure: %s Millibars" % sense.pressure)

humidity = sense.get_humidity()
print("Humidity: %s %%rH" % humidity)

sense = SenseHat()
temp = sense.get_temperature()
print("Temperature: %s C" % temp)

# alternatives
print("Alt-1: Temperature: %s C" % sense.temp)
print("Alt-1: Temperature: %s C" % sense.temperature)