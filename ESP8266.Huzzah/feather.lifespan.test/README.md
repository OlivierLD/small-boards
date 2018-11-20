This code intends to test the life span of a Feather (with ESP8266 module) powered by a LiPo battery.

The result may vary, depending on the battery you use.

This requires a small REST server to be running on some machine on the same network as the Feather,
with a service implementing resources like `POST /feather/lifespan`, as seen in the code.

The test will ping the REST server, posting on the resource above a payload formatted `HH:MM:SS`, like `01:15:06` every second or so, until it dies.
The last posted payload reflects the lifespan of the Feather.


