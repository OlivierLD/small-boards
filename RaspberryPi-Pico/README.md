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
Hit `[Return]` if needed to get to the prompt.  
Type `[Ctrl+D]` at the prompt:
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
Hit `[Return]` if needed to get to the prompt.  
Then, just like above, try that:
```
>>> from machine import Pin
>>> led = Pin(25, Pin.OUT)
>>> led.value(1)
>>> led.value(0)
```
To exit `minicom`, do a `[Ctrl-A]` then `X`.

#### To flash a Python script on the Pico
- Use `rshell` (`pip3 install rshell`)
```
$ rshell -p /dev/tty.usbmodem0000000000001 --buffer-size 512
Using buffer-size of 512
Connecting to /dev/tty.usbmodem0000000000001 (buffer-size 512)...
Trying to connect to REPL  connected
Testing if ubinascii.unhexlify exists ... Y
Retrieving root directories ... 
Setting time ... Jan 26, 2021 08:43:23
Evaluating board_name ... pyboard
Retrieving time epoch ... Jan 01, 1970
Welcome to rshell. Use Control-D (or the exit command) to exit rshell.
/Users/olivierlediouris/repos/small-boards/RaspberryPi-Pico/MicroPython> ls /pyboard
/Users/olivierlediouris/repos/small-boards/RaspberryPi-Pico/MicroPython> cp ./basic_101.py /pyboard/main.py
/Users/olivierlediouris/repos/small-boards/RaspberryPi-Pico/MicroPython>
```
> The command above was done from a Mac, hence the port name `/dev/tty.usbmodem0000000000001`.
> From a Raspberry Pi, that would be like `/dev/ttyACM0`, from Windows, like `COM4`, etc.

> Notice that the file at the root of the board (in `pyboard`) must be named `main.py`.

After doing the command above, unplug and re-plug the power of the Pico, and
the script should start (in this case, the led is blinking forever).

 
## CircuitPython


## C/C++

