## M5Stick-C
- See <http://m5edu.com/Product/m5stick-c-micro-core/>
- See <http://forum.m5stack.com/topic/41/lesson-1-1-lcd-graphics>
- Arduino libs instructions [here](https://www.instructables.com/id/How-to-Program-ESP32-M5Stack-StickC-With-Arduino-I/)
- And [here](https://docs.m5stack.com/#/en/quick_start/m5stickc/m5stickc_quick_start_with_arduino_Windows?id=_3-install-m5stack-library)
  - Use board `EPS32 Arduino` > `M5Stick-C`
  - Use Upload Speed `115200`

Hostname is `espressif`.

Example: Client for the Nav Server (`HttpClient.ino`)

|    B & W            |    Colors           |
|:-------------------:|:-------------------:|
| ![One](./M5.01.jpg) | ![Two](./M5.02.jpg) |

#### To fit your environment
- You need a `NavServer` or an `NMEA.multiplexer` to be available, possibly emitting its own network (aka hotspot), as explained [here](https://github.com/OlivierLD/raspberry-coffee/tree/master/NMEA.mux.WebUI), [here](https://github.com/OlivierLD/raspberry-coffee/blob/master/NMEA.mux.WebUI/small.server.extended/README.md), and around...
- Modify the sketch's code appropriately, in my case:
```C
const char* SSID = "RPi-Gateway";           // your network SSID (name)
const char* PASSWORD = "raspberrypi";       // your network password
const char* SERVER_NAME = "192.168.50.10";  // For REST requests, Nav Server
```

Upload the sketch on the `M5Stcik-C`, and you're good to go! You have debug messages in the `Serial` console.

### Apple...
Apple did it again, what was working before does not anymore (not to mention all the cables and connectors). On the last OS (Catalina), the serial port where the M5Stick-C is connected cannot be used to upload a new sketch.
Again, use a Raspberry Pi, and you'll be just fine.

---
