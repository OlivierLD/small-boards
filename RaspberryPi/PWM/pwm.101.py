'''
Control the Brightness of LED using PWM on Raspberry Pi
Resistor 220 or 230 Ohms on the wiring schema
requires a 
python3 -m pip gpiozero.
'''

# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-pwm-python/

from gpiozero import PWMLED
from time import sleep

# led = PWMLED(14)  # GPIO14, pin #8
led = PWMLED(18)  # GPIO18, pin #12

led.value = 1   # LED fully on
sleep(1)
led.value = 0.5  # LED half-brightness
sleep(1)
led.value = 0    # LED fully off
sleep(1)

try:
  # fade in and out forever
  while True:
    #fade in
    for duty_cycle in range(0, 100, 1):
      led.value = duty_cycle/100.0
      sleep(0.05)

    #fade out
    for duty_cycle in range(100, 0, -1):
      led.value = duty_cycle/100.0
      sleep(0.05)
      
except KeyboardInterrupt:
  print("Stop the program and turning off the LED")
  led.value = 0
  pass

print("Done with PWM.")
