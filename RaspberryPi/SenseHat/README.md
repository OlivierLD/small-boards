# Sense HAT
See the repo at https://github.com/astro-pi/python-sense-hat/tree/master  
Doc [here](https://sense-hat.readthedocs.io/en/latest/)

All the code is to be executed on a Raspberry Pi, with a Sense HAT attached on top.

To clone only a part of a git repo:

> - Clone the repository (without checking out files yet)  
>   `git clone --no-checkout https://github.com/xxx/yy.git`   
>   `cd ui`
> - Enable sparse checkout in cone mode  
>   `git sparse-checkout init --cone`
> - Specify the folder you want  
>   `git sparse-checkout set apps/www`
> - Check out the branch  
>   `git checkout main`

```python
from sense_hat import SenseHat

sense = SenseHat()

pressure = sense.get_pressure()
print("Pressure: %s Millibars" % pressure)

# alternatives
print(sense.pressure)

humidity = sense.get_humidity()
print("Humidity: %s %%rH" % humidity)

sense = SenseHat()
temp = sense.get_temperature()
print("Temperature: %s C" % temp)

# alternatives
print(sense.temp)
print(sense.temperature)
```