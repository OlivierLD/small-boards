# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-analog-inputs-python-mcp3008/

import gpiozero
from gpiozero import PWMLED, MCP3008
from time import sleep


MCP3008_CHANNEL: int = 0
LED_PIN: int = 14
# create an object called pot that refers to MCP3008 channel 0
pot: gpiozero.spi_devices.MCP3008 = MCP3008(MCP3008_CHANNEL)

# create a PWMLED object called led that refers to GPIO 14
led = PWMLED(LED_PIN)
print(f"LED is a {type(led)}")

try:
    while True:
        if (pot.value < 0.001):
            led.value = 0
        else:
            led.value = pot.value
        print(pot.value)
        sleep(0.1)
except KeyboardInterrupt:
    print("\nTurning led off")
    led.value = 0
    pass

print("Bye!")

