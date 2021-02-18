# import machine
from machine import UART

#
# See https://docs.micropython.org/en/latest/library/machine.UART.html
# THIS DOES NOT WORK ON THE PICO
#

uart = UART(1, 4800)                         # init with given baudrate
uart.init(4800, bits=8, parity=None, stop=1, tx=1, rx=2) # init with given parameters

#uart.read(10)       # read 10 characters, returns a bytes object
#uart.read()         # read all available characters
#uart.readline()     # read a line
uart.readinto(buf)  # read and store into the given buffer
# uart.write('abc')   # write the 3 characters

print("Read: {}".buf)

