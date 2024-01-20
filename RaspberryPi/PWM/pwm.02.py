'''
Control the Brightness of LED using PWM on Raspberry Pi
Resistor 220 or 230 Ohms on the wiring schema
requires a 
$ python3 -m pip gpiozero
or 
$ pip3 install gpiozero
'''

# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-pwm-python/

import gpiozero
from gpiozero import PWMLED
from time import sleep

led: gpiozero.output_devices.PWMLED = PWMLED(14)  # GPIO14, pin #8
# led: gpiozero.output_devices.PWMLED = PWMLED(18)  # GPIO18, pin #12

print("-- First test --")
print("LED fully on")
led.value = 1   # LED fully on
sleep(5)
print("LED half on")
led.value = 0.5  # LED half-brightness
sleep(5)
print("LED fully off")
led.value = 0    # LED fully off
sleep(5)
print("-- End of first test --")

try:
  # fade in and out forever
  while True:
    # fade in (0 -> 1)
    for duty_cycle in range(0, 100, 1):
      print(f"Setting value to {duty_cycle/100.0}")
      led.value = duty_cycle/100.0
      sleep(0.05)

    # fade out (1 -> 0)
    for duty_cycle in range(100, 0, -1):
      print(f"Setting value to {duty_cycle/100.0}")
      led.value = duty_cycle/100.0
      sleep(0.05)
      
except KeyboardInterrupt:
  print("Stop the program and turning the LED off")
  led.value = 0
  pass

print("Done with PWM.")
