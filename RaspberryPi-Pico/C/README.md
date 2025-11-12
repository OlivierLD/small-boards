# Raspberry Pi Pico in C
Coming...

- Raspberry Pico [C/C++ SDK](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html).
- [Getting started](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)

## On Linux (including Raspberry Pi)
```
$ sudo apt install python3 git tar build-essential
```
if needed:
```
$ sudo apt install screen
```

In this [doc mentionned above](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf), look for `Configure your environment via Script`, and follow the instructions.
Execute the `$ ./pico_setup.sh`, it should do all the job. It actually does what's 
described in `Manually Configure your Environment`.  
The SDK is definitely heaver than the board itself 😉.  
Reboot after the install.

The Pico-SDK is in its own [repo](https://github.com/raspberrypi/pico-sdk).

In _MS Visual Studio_:  
You can find the extension in the VS Code Extensions Marketplace. Search for the **Raspberry Pi Pico** extension,
published by Raspberry Pi. Click the Install button to add it to VS Code.  
Do the same with the **Serial Console** extension.

Next, go to the section `Use the CLI to Blink an LED in C`.

## A first test
From the folder `C/test`,
```
$ cp ~/pico/pico-sdk/external/pico_sdk_import.cmake .
```
then
```
$ mkdir build
$ cd build
$ export PICO_SDK_PATH=~/pico/pico-sdk
$ cmake ..
$ make
```
... Then the `uf2` file is in your current (`build`) directory.

Drag it onto the corresponding port, then
```
$ [sudo] minicom -D /dev/ttyACM1 -b 115200
```
Hint: to exit `minicom`:
```
[Ctrl-A], x, [Enter]
```

or with `screen`:
```
$ screen /dev/ttyACM1 115200
```
To exit `screen`:
```
[Ctrl-A], d
```


You can also see this in the _Serial Console_ (installed as mentioned before) in _MS Visual Studio_.

## TODO
- See this `make -j4`...


<!--
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

-->

---