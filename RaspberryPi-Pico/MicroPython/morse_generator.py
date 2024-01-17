from machine import Pin
import sys
import time
"""
Translate a string (hard-coded below)
into its morse equivalent.
"""
# Warning: MicroPython does not support print(f"...")

ON = 1
OFF = 0


led = Pin(25, Pin.OUT)  # Built-in led on Raspberry Pi Pico
# led = Pin('Y12')  # To use with https://micropython.org/unicorn/

def dash() -> None:
    global led
    led(ON)
    time.sleep_ms(500)  # time.sleep(0.5) would do it too.
    led(OFF)


def dot() -> None:
    global led
    led(ON)
    time.sleep_ms(250)
    led(OFF)


def blink_the_led(code: str) -> None:
    for idx in range(0, len(code)):
        elem = code[idx:idx+1]
        if elem == ".":
            dot()
        elif elem == "-":
            dash()
        time.sleep_ms(100)  # Between signs in a letter


MORSE_CODE = {
    "A": ".-",
    "B": "-...",
    "C": "-.-.",
    "D": "-..",
    "E": ".",
    "F": "..-.",
    "G": "--.",
    "H": "....",
    "I": "..",
    "J": ".---",
    "K": "-.-",
    "L": ".-..",
    "M": "--",
    "N": "-.",
    "O": "---",
    "P": ".--.",
    "Q": "--.-",
    "R": ".-.",
    "S": "...",
    "T": "-",
    "U": "..-",
    "V": "...-",
    "W": ".--",
    "X": "-..-",
    "Y": "-.--",
    "Z": "--..",
    "0": "-----",
    "1": ".----",
    "2": "..---",
    "3": "...--",
    "4": "....-",
    "5": ".....",
    "6": "-....",
    "7": "--...",
    "8": "---..",
    "9": "----."
}


def translate(to_translate: str) -> None:
    to_translate = to_translate.upper()
    for idx in range(0, len(to_translate)):
        # letter = to_translate[idx:idx+1]
        letter = to_translate[idx]
        if letter in MORSE_CODE:
            code = MORSE_CODE[letter]
            print("{} = {}".format(letter, code))
            blink_the_led(code)
        else:
            print("\t[{}] not in the code".format(letter))
        time.sleep_ms(200) # between letters in the text to translate
        

if __name__ == '__main__':
    print("Using Python version {}".format(sys.version))
    to_translate = "Hello World, 1, 2, 3"
    translate(to_translate)
    print("\n\tDone.")

