from machine import Pin
import sys
import time

# MicroPython does not support print(f"...")
print("Using Python version {}".format(sys.version))


led = Pin(25, Pin.OUT)  # Built-in led on Raspberry Pi Pico
# led = Pin('Y12')  # To use with https://micropython.org/unicorn/

def dash():
    global led
    led(1)
    time.sleep_ms(500)
    led(0)


def dot():
    global led
    led(1)
    time.sleep_ms(250)
    led(0)


def blink_the_led(code):
    for idx in range(0, len(code)):
        elem = code[idx:idx+1]
        if elem == ".":
            dot()
        elif elem == "_":
            dash()
        time.sleep_ms(100)  # Between signs


MORSE_CODE = {
    "A": "._",
    "B": "_...",
    "C": "_._.",
    "D": "_..",
    "E": ".",
    "F": ".._.",
    "G": "__.",
    "H": "....",
    "I": "..",
    "J": ".___",
    "K": "_._",
    "L": "._..",
    "M": "__",
    "N": "_.",
    "O": "___",
    "P": ".__.",
    "Q": "__._",
    "R": "._.",
    "S": "...",
    "T": "_",
    "U": ".._",
    "V": "..._",
    "W": ".__",
    "X": "_.._",
    "Y": "_.__",
    "Z": "__..",
    "0": "_____",
    "1": ".____",
    "2": "..___",
    "3": "...__",
    "4": "...._",
    "5": ".....",
    "6": "_....",
    "7": "__...",
    "8": "___..",
    "9": "____."
}

to_translate = "Hello World"
to_translate = to_translate.upper()

for idx in range(0, len(to_translate)):
    # letter = to_translate[idx:idx+1]
    letter = to_translate[idx]
    if letter in MORSE_CODE:
        code = MORSE_CODE[letter]
        print("{} = {}".format(letter, code))
        blink_the_led(code)
    else:
        print("\t{} not in the code".format(letter))
    time.sleep_ms(200) # between letters

print("Done")

