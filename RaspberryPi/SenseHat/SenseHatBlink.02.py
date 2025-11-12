#!/usr/bin/python
# import sys
import time

from sense_hat import SenseHat

sense = SenseHat()

# Led matrix is 8x8

ON = [255, 255, 255]
OFF = [0, 0, 0]

RED = [255, 0, 0]
WHITE = [255, 255, 255]
BLACK = [0, 0, 0]

BANG_SIGN = [
  BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,
  BLACK, BLACK, BLACK, RED,   BLACK, BLACK, BLACK, BLACK,
  BLACK, BLACK, RED,   BLACK, RED,   BLACK, BLACK, BLACK,
  BLACK, RED,   WHITE, BLACK, WHITE, RED,   BLACK, BLACK,
  RED,   WHITE, WHITE, BLACK, WHITE, WHITE, RED,   BLACK,
  RED,   WHITE, WHITE, WHITE, WHITE, WHITE, RED,   BLACK,
  RED,   WHITE, WHITE, BLACK, WHITE, WHITE, RED,   BLACK,
  RED,   RED,   RED,   RED,   RED,   RED,   RED,   BLACK
]

# msleep = lambda x: time.sleep(x / 1000.0). # This is a nice one.

sense.set_rotation(90)  # Rotate display if needed

try:
  while True:
    sense.set_pixels(BANG_SIGN)
    time.sleep(1)
    sense.clear(OFF)
    time.sleep(1)
except KeyboardInterrupt:
  sense.clear()  # to clear the LED matrix

print("Exiting SenseHatBlink.02.py")
print("Bye now!")