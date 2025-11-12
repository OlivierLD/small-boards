#!/usr/bin/python
from sense_hat import SenseHat

sense = SenseHat()
sense.set_rotation(90)
red = (255, 0, 0)

try:
  while True:
    sense.show_message("Warning!", text_colour=red)
except KeyboardInterrupt:
  sense.clear()  # to clear the LED matrix

print("Exiting ", __file__)
print("Bye now!")