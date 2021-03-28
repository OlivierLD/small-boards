# Arduino (and friends)
Doc at <https://www.arduino.cc/reference/en/>

## Arduino, read Serial data

> Note: if you have trouble uploading your sketch(es), do unplug the Serial data provider (like the GPS), and try again. 
> It worked for me.

### Wiring
![Wiring](./SerialDump/Arduino.GPS_bb.png)

Here the GPS is an [Adafruit Ultimate GPS Breakout - 66 channel w/10 Hz updates - Version 3](https://www.adafruit.com/product/746)

### The code
- `SerialDump.ino` shows the basics. 
- `SerialReader.ino` format the NMEA Sentences, and spit them out in the Serial Console:
```
07:50:39.415 -> Setup completed
07:50:42.417 -> $PGACK,103*40
07:50:42.417 -> $PGACK,105*46
07:50:42.453 -> $PMTK011,MTKGPS*08

. . .

08:54:51.003 -> RMC Sentence:$GPRMC,155447.000,A,3744.9380,N,12230.4221,W,0.01,169.54,280321,,,D*7A
08:54:51.003 -> Position:
08:54:51.003 -> 	Lat:37.75
08:54:51.003 -> 	Lng:-122.51
08:54:51.003 -> Velocity:
08:54:51.003 -> 	cog:169.54
08:54:51.003 -> 	sog:0.01
08:54:51.003 -> Time:
08:54:51.003 -> 	UTC:28-MAR-2021 15:54:47.00
08:54:51.003 -> D:0.00
08:54:51.003 -> .................................................................................
08:56:09.804 -> .................................................................................
08:57:29.704 -> .................................................................................
08:58:50.177 -> ......

. . .
```

#### Energy Consumption
In this config, the Arduino consumes between 0.04 and 0.06 Amps.
