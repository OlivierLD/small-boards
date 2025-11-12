# Sense HAT
See the repo at https://github.com/astro-pi/python-sense-hat/tree/master  
Doc [here](https://sense-hat.readthedocs.io/en/latest/)

All the code is to be executed on a Raspberry Pi, with a Sense HAT attached on top.

To clone only a part of a git repo:

> - Clone the repository (without checking out files yet)  
>   `git clone --no-checkout https://github.com/xxx/yy.git`   
>   `cd yy`
> - Enable sparse checkout in cone mode  
>   `git sparse-checkout init --cone`
> - Specify the folder you want  
>   `git sparse-checkout set apps/www`
> - Check out the branch  
>   `git checkout main`

Here, we're talking about the git repo at `https://github.com/OlivierLD/small-boards`,
the directory we need is `RaspberryPi/SenseHat`.  
The commands would be here, in our very case:
```
git clone --no-checkout https://github.com/OlivierLD/small-boards  
cd small-boards
git sparse-checkout init --cone
git sparse-checkout set RaspberryPi/SenseHat
git checkout
```

After installing the Sense-HAT python packages on the Raspberry Pi, 
they should be in `/usr/lib/python3/dist-packages/sense_hat/sense_hat.py`.

To refer to the packages in a specific folder, see https://www.geeksforgeeks.org/python/how-to-import-a-python-module-given-the-full-path/.

System variable
```
PYTHONPATH=/usr/lib/python3/dist-packages/sense_hat
```
or in the Python code
```python
# importing module
import sys

# appending a path
sys.path.append('/usr/lib/python3/dist-packages/sense_hat')

from sense_hat import SenseHat

. . .

```

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