#!/usr/bin/python
#
# Displaying images.
#
import time
from sense_hat import SenseHat

sense = SenseHat()
sense.set_rotation(90)
red = (255, 0, 0)

try:
  sense.clear()
  while True:
    for i in range(0, 13):
      name = "emojis/{:02d}.png".format(i)
      print("Loading image: {}".format(name))
      sense.load_image(name)
      time.sleep(2)
except KeyboardInterrupt:
  sense.clear()  # to clear the LED matrix

print("Exiting ", __file__)
print("Bye now!")