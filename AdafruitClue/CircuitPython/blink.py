import board
import digitalio
import time

led = digitalio.DigitalInOut(board.D17)
led.direction = digitalio.Direction.OUTPUT

print("Starting, look behind the board")
# on = True

while True:
    # if on:
    #    print("Bim")
    #    on = False
    # else:
    #    print("Bam")
    #    on = True
    led.value = True
    time.sleep(0.5)
    led.value = False
    time.sleep(0.5)
