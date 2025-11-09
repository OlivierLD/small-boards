# Very first test for the FireBeetle ESP32

Using board `FireBeetle-ESP32`.

When uploading, an error occurred: 
```
A fatal error occurred: Unable to verify flash chip connection (Invalid head of packet (0xE0): Possible serial noise or corruption.).
```
Changing the upload speed to `115200` fixed the issue.  
In the menu `Tools` > `Upload Speed`.

--- 

The code in the `setup`:
```
Serial.begin(115200);
```

`115200` may produce un-readable characters... `9600` is the default speed for the Serial Console...
Make sure this matches the console's speed (aka baud rate).

---