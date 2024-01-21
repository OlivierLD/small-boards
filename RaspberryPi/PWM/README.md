# Pulse Width Modulation (aka PWM)

- See [this](https://passe-coque.com/admin/sql/index.html).

![Schema](./RasPi.led.png)

- The resistor is a 220 &Omega; one. **DO NOT OMIT THE RESISTOR**!   
Or you could get yourself a new Raspberry Pi.
- The physical pin #6 is the `GND` one
- The physical pin #12 is also named `GPIO18`

To see the led glow up and down, run:
```
$ python3 pwm.01.py
```
`Ctrl-C` will stop the program.

Also try (works the same, with other Python modules)
```
$ python3 pwm.02.py
```

**_Warning_**: all the leds are not the same... I've had wierd blinkings with some (with the exact same setting).

## Servos and PWM
See this <https://www.learnrobotics.org/blog/raspberry-pi-servo-motor/>  



---
