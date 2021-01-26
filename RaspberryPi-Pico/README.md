# Raspberry Pi Pico
Can be programmed several ways.


- <https://www.raspberrypi.org/documentation/pico/getting-started/>

## MicroPython
- <https://datasheets.raspberrypi.org/pico/sdk/pico_python_sdk.pdf>
- <https://core-electronics.com.au/tutorials/getting-started-with-raspberry-pi-pico.html>

If `minicom` is not available (like on a Mac), use `screen`.  

### From a Mac
The port for the Raspberry Pi Pico could be like `/dev/tty.usbmodem0000000000001`.  

```
$ screen /dev/tty.usbmodem0000000000001 9600
>>>
```
Type [Ctrl+D] at the prompt:
```
>>> 
MPY: soft reboot
MicroPython v1.13-290-g556ae7914 on 2021-01-21; Raspberry Pi Pico with RP2040
Type "help()" for more information.
>>> 
```
Then try `help()`, and this kind of things, you _**are**_ in a Python REPL.  
Good!  
Try that:
```
>>> from machine import Pin
>>> led = Pin(25, Pin.OUT)
>>> led.value(1)
>>> led.value(0)
```
To exit `screen`, do a `[Ctrl-a]` then `d`.

- See <https://pbxbook.com/other/mac-tty.html>

### From a Raspberry Pi
You will need `minicom`:
```
$ sudo apt install minicom
```
then (with the Pico connected, probably on `/dev/ttyACM0`)
```
$ minicom -o -D /dev/ttyACM0
Welcome to minicom 2.7.1

OPTIONS: I18n 
Compiled on Aug 13 2017, 15:25:34.
Port /dev/ttyACM0, 16:58:34

Press CTRL-A Z for help on special keys


>>> 
```
Then, just like above, try that:
```
>>> from machine import Pin
>>> led = Pin(25, Pin.OUT)
>>> led.value(1)
>>> led.value(0)
```
To exit `minicom`, do a `[Ctrl-A]` then `X`.

## CircuitPython


## C/C++

