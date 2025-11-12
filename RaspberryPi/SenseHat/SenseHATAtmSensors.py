from sense_hat import SenseHat

sense = SenseHat()

pressure = sense.get_pressure()
print("Pressure: %s Millibars" % pressure)

# alternatives
print(sense.pressure)

humidity = sense.get_humidity()
print("Humidity: %s %%rH" % humidity)

sense = SenseHat()
temp = sense.get_temperature()
print("Temperature: %s C" % temp)

# alternatives
print(sense.temp)
print(sense.temperature)