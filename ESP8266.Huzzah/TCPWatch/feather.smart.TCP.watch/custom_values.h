/*
 * Define server connection parameters used in huzzah.smart.TCP.watch.ino
 */

#ifdef __PI_NET__
#define _SSID "RPi-Gateway"
#define _PASSWORD "raspberrypi"
#define _HOST "192.168.50.10"
#define _HTTP_PORT 9999
#endif

#ifdef __SONIC_AT_HOME__
#define _SSID "Sonic-00e0_EXT"
#define _PASSWORD "xxxxxc7831"
#define _HOST "192.168.42.8"
#define _HTTP_PORT 9999
#endif

#ifdef __SAN_JUAN__
#define _SSID "BEC2BB"
#define _PASSWORD "C9AA72E6"
#define _HOST "192.168.1.103"
#define _HTTP_PORT 9999
#endif

// Etc...
