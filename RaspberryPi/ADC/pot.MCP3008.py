# Just read the pto value, thnrough the MCP3008

import gpiozero 
from gpiozero import MCP3008
from time import sleep

MCP3008_CHANNEL: int = 0
# create an object called pot that refers to MCP3008 channel 0
pot: gpiozero.spi_devices.MCP3008 = MCP3008(MCP3008_CHANNEL)
# print(f"Pot is a {type(pot)}")

prev_value: float = 0

try:
    while True:
        if (abs(prev_value - pot.value) > 0.001):
            print(f"Pot. value: {pot.value}")
        prev_value = pot.value
        # print(f"Pot. value: {pot.value}")  # [0..1024]
        sleep(0.1)
except KeyboardInterrupt:
    print("\nBye now!")
    pass

print("Done with MCP3008")
