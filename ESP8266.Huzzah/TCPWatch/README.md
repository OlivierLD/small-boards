## Building a TCP watch - WIP 🚧.
The idea is to have a watch-like device that does _not_ need a smart phone to connect to a network.

For now, it seems that _all_ so-called smart watches need a cell-phone to connect to (via BlueTooth) to get to the net.

Ah yes, and why not a dish-washer, or a police car 🚓, or an helicopter 🚁 ???

Boards like the ESP8266 (and similar, like the Adafruit Huzzah - less than $10 in Nov-2018, Adafruit Feather Huzzah, etc) have WiFi capabilities that allow you to skip the need for a
cell-phone between you and the network.

OK, the app running on the ESP8266 might need some parameters, some customization, some configuration... fine.
Well, use a USB cable, plug it in your laptop, and off you go.
That's done here through the Arduino IDE, it works just fine.
It might not be as user-friendly as a nice web-page, but I'm working on it.

I'm not there yet, but this could be a cheap approach.

The Pebble was a cheap smart-watch, the first option was about $100. But after it got acquired by Fitbit,
it was gone - along with the nice Cloud IDE they were providing. The cheapest of Fitbit is more than $200,
and this is the kind of devices that tells you to stand up every time you sit down. Not needed.

It will be using REST over HTTP, TCP will come later, if needed.

The ESP8266/Huzzah has built-in WiFi capabilities, and can deal with an OLED screen like the `SSD1306`.

The starting point was [this document](https://learn.adafruit.com/huzzah-weather-display?view=all).

The ESP8266 will ping the [NavServer/NMEA.multiplexer](https://github.com/OlivierLD/raspberry-coffee/blob/master/NMEA.multiplexer/README.md) with a request like `GET /mux/cache?option=txt`.

## Several implementations

- [Huzzah/ESP8266 with a 128x64 oled display](./huzzah.smart.TCP.watch/README.md)
- [Feather-Huzzah/ESP8266 with a 128x64 oled display](./huzzah.smart.TCP.watch/README.md#prototyping-with-an-adafruit-feather-huzzahesp8266)
- [Feather-Huzzah/ESP8266 with a 128x32 stacked oled display](./feather.smart.TCP.watch/README.md)

---
