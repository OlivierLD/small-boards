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
print("Temperature: %s\u00b0C" % temp)

# alternatives
print("Alt-1: Temperature: %s\u00b0C" % sense.temp)
print("Alt-1: Temperature: %s\u00b0C" % sense.temperature)