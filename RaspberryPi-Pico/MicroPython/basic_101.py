from machine import Pin, Timer
import sys

print("Python version {}".format(sys.version))

led = Pin(25, Pin.OUT)
timer = Timer()


def tick(timer: Timer) -> None:
    global led
    led.toggle()
    print('Toggle LED')

timer.init(freq=1, mode=Timer.PERIODIC, callback=tick)
