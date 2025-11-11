# Raspberry Pi Pico in C
Coming...

- Raspberry Pico [C/C++ SDK](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html).
- [Getting started](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)

## On Linux (including Raspberry Pi)
```
$ sudo apt install python3 git tar build-essential
```

In this [doc mentionned above](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf), look for `Configure your environment via Script`, and follow the instructions.
Execute the `$ ./pico_setup.sh`, it should do all the job. It actually does what's 
described in `Manually Configure your Environment`.  
The SDK is definitely heaver than the board itself 😉.  
Reboot after the install.

The Pico-SDK is in its own [repo](https://github.com/raspberrypi/pico-sdk).

In MS Visual Studio:  
You can find the extension in the VS Code Extensions Marketplace. Search for the Raspberry Pi Pico extension,
published by Raspberry Pi. Click the Install button to add it to VS Code.

Next, go to the section `Use the CLI to Blink an LED in C`.


### PicoTool ?
```
$ git clone https://github.com/raspberrypi/picotool.git
$ cd picotool
```

```
$ sudo apt install libusb-1.0-0-dev
```

#### Build picotool
```
$ mkdir build
$ cd build
$ export PICO_SDK_PATH=~/pico/pico-sdk
$ cmake ../
$ make
```