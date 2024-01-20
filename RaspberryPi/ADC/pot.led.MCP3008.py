# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-analog-inputs-python-mcp3008/

import gpiozero 
from gpiozero import PWMLED, MCP3008
from time import sleep


MCP3008_CHANNEL: int = 0
LED_PIN: int = 18   # 14
# create an object called pot that refers to MCP3008 channel 0
pot: gpiozero.spi_devices.MCP3008 = MCP3008(MCP3008_CHANNEL)

# create a PWMLED object called led that refers to GPIO 14
led: gpiozero.output_devices.PWMLED = PWMLED(LED_PIN)
# print(f"LED is a {type(led)}")

print(f"For the led, using pin {LED_PIN}, reading MCP3008 channel #{MCP3008_CHANNEL}")

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

prev_value: float = -10

print("Entering POT loop. Ctrl-C to exit.")
try:
    while True:
        if (pot.value < 0.001):
            led.value = 0
        else:
            led.value = pot.value
        if (abs(prev_value - pot.value) > 0.001):
            print(f"Pot. value: {pot.value}")
        prev_value = pot.value
        sleep(0.1)
except KeyboardInterrupt:
    print("\nTurning led off")
    led.value = 0
    pass

print("Bye!")

