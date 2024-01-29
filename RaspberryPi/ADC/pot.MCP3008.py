# Just read the pot value, through the MCP3008

import gpiozero 
from gpiozero import MCP3008
from time import sleep

MCP3008_CHANNEL: int = 0  # in [0..7]
# create an object called pot that refers to MCP3008 channel 0
pot: gpiozero.spi_devices.MCP3008 = MCP3008(MCP3008_CHANNEL)
# print(f"Pot is a {type(pot)}")

prev_value: float = -10

# Notice: pot.value goes from 0 to 1, and NOT from 0 to 1024 !!
try:
    while True:
        if (abs(prev_value - pot.value) > 0.001):  # value has changed
            print(f"Pot. value: {pot.value}")
        prev_value = pot.value
        # print(f"Pot. value: {pot.value}")  # [0..1]
        sleep(0.1)
except KeyboardInterrupt:
    print("\nBye now!")
    pass

print("Done with MCP3008")
