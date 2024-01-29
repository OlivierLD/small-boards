# Read pot values from a joystick, through the MCP3008
# Neutral pos is U/D: 0.5, L/R: 0.5

import gpiozero 
from gpiozero import MCP3008
from time import sleep

MCP3008_CHANNEL_0: int = 0  # in [0..7]
MCP3008_CHANNEL_1: int = 1  # in [0..7]
# create an object called pot that refers to MCP3008 channel 0, and 1
pot_up_down: gpiozero.spi_devices.MCP3008 = MCP3008(MCP3008_CHANNEL_0)
pot_left_right: gpiozero.spi_devices.MCP3008 = MCP3008(MCP3008_CHANNEL_1)
# print(f"Pot is a {type(pot)}")

prev_value_up_down: float    = -10
prev_value_left_right: float = -10

# Notice: pot.value goes from 0 to 1, and NOT from 0 to 1024 !!
try:
    while True:
        if (abs(prev_value_up_down - pot_up_down.value) > 0.001):  # value has changed
            print(f"Up-Down    value: {pot_up_down.value}")
        if (abs(prev_value_left_right - pot_left_right.value) > 0.001):  # value has changed
            print(f"Left-Right value: {pot_left_right.value}")

        prev_value_up_down    = pot_up_down.value
        prev_value_left_right = pot_left_right.value
        # print(f"Pot. value: {pot.value}")  # [0..1]
        sleep(0.1)
except KeyboardInterrupt:
    print("\nBye now!")
    pass

print("Done with MCP3008 and joystick")
