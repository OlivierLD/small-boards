#!/usr/bin/python
# import sys
import time

from sense_hat import SenseHat

sense = SenseHat()

ON = [255, 255, 255]
OFF = [0, 0, 0]

# msleep = lambda x: time.sleep(x / 1000.0)

while True:
    sense.clear(ON)
    time.sleep(1)
    sense.clear(OFF)
    time.sleep(1)
