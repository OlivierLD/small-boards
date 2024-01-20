# Analog to Digital Converter (ADC)

Good doc at <https://randomnerdtutorials.com/raspberry-pi-analog-inputs-python-mcp3008/>

Make sure SPI is enabled (you can also use `raspi-config`)
```
$ sudo raspi-config nonint do_spi 0
```

> Something to notice:  
> In C (or Java, or others), the values returned when reading a channel from the MCP3008 are in `[0..1023]`. The MCP3008 is a 10-bit device, and `2^10 = 1024`.  
> Here - in the Python code - the values are in `[0..1]`.  
> Something not to forget.

## With a potentiometer only
![Pot Only](./pot.mcp3008_bb.only.png)

Then run 
```
$ python3 pot.MCP3008.py
```

## With a potentiometer _AND_ a led (PWM powered)
![Pot and Led](./pot.mcp3008_bb.with.led.png)

Make sure the `LED_PIN` is set correctly in the code (`pot.led.MCP3008.py`).  
Then run 
```
$ python3 pot.led.MCP3008.py
```
The led will glow, according to the value of the potentiometer.
