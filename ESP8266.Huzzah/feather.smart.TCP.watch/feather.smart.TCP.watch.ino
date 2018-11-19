/**
   Simple HTTP webclient REST, for the NavServer,
   for Feather-Huzzah/ESP8266.

   Main class of what will be the TCP watch.

   Sends REST requests to the NavServer to get navigation data (see REST_REQUEST variable)
   That one spits out data on the Serial console, and on an oled screen SSD1306 124x68.

   @author Olivier LeDiouris
*/
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// #define DEBUG // Uncomment for more Serial output.

/* ----- Customizable Data ----- */
// Network and Host Names definitions, customize if necessary
#define __PI_NET__       // 192.168.127.1 on Pi-Net
//#undef __PI_NET__
//#define __SONIC_AT_HOME__   // 192.168.42.4 on Sonic-00e0
#undef __SONIC_AT_HOME__

#include "custom_values.h" // Contains _SSID, _PASSWORD, _HOST, _HTTP_PORT, used below.

const char* SSID     = _SSID;
const char* PASSWORD = _PASSWORD;

const char* HOST = _HOST;
const int HTTP_PORT = _HTTP_PORT;

const char* REST_REQUEST = "/mux/cache?option=txt"; // txt, not json.

const int BETWEEN_LOOPS = 500; // in milli-sec.
/* ----- End of Customizable Data ----- */

//#define RST 2

Adafruit_SSD1306 ssd1306 = Adafruit_SSD1306(128, 32, &Wire);

// OLED FeatherWing buttons map to different pins depending on board:
#if defined(ESP8266)
#define BUTTON_A  0
#define BUTTON_B 16
#define BUTTON_C  2
#define LED       0
#elif defined(ESP32)
#define BUTTON_A 15
#define BUTTON_B 32
#define BUTTON_C 14
#define LED      13
#elif defined(ARDUINO_STM32_FEATHER)
#define BUTTON_A PA15
#define BUTTON_B PC7
#define BUTTON_C PC5
#define LED PB5
#elif defined(TEENSYDUINO)
#define BUTTON_A  4
#define BUTTON_B  3
#define BUTTON_C  8
#define LED 13
#elif defined(ARDUINO_FEATHER52)
#define BUTTON_A 31
#define BUTTON_B 30
#define BUTTON_C 27
#define LED 17
#else // 32u4, M0, M4, and 328p
#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5
#define LED      13
#endif

// Data from REST server
float bsp, lat, lng, sog;
int cog, year, month, day, hour, mins, sec;
String date;

const int NS = 1;
const int EW = 2;

char* toDegMin(float data, int type) {
  int sign = (data >= 0) ? 1 : -1;
  float absData = data >= 0 ? data : -data; // abs returns an int... TODO Try fabs
  int intPart = (int)absData;
  float minSec = (absData - intPart) * 60;
#ifdef DEBUG
  Serial.print("Raw:"); Serial.print(data); Serial.print(" -> "); Serial.println(absData);
  Serial.print("Int:"); Serial.println(intPart);
  Serial.print("MinSec:"); Serial.println(minSec);
#endif
  char degMinVal[64];
  sprintf(degMinVal, "%c %d %.2f'", (type == NS ? (sign == 1 ? 'N' : 'S') : (sign == 1 ? 'E' : 'W')), intPart, minSec);
#ifdef DEBUG
  Serial.print("Deg Minutes:");
  Serial.println(degMinVal);
#endif
  return degMinVal;
}

int screenIndex = 0;

void repaint() {
  ssd1306.clearDisplay();

  ssd1306.setTextColor(WHITE);
  ssd1306.setTextSize(1);
//ssd1306.setTextSize(2);

  char dataBuffer[128];

  ssd1306.setCursor(0, 0);
  switch (screenIndex) {
    case 0:
      ssd1306.println("--- Position ---"); 
      sprintf(dataBuffer, "L: %s", toDegMin(lat, NS));
      ssd1306.println(dataBuffer);
      sprintf(dataBuffer, "G: %s", toDegMin(lng, EW));
      ssd1306.println(dataBuffer);
      break;
    case 1:
      ssd1306.println("--- Displacement ---");
      sprintf(dataBuffer, "SOG: %.2f kts", sog);
      ssd1306.println(dataBuffer);
      sprintf(dataBuffer, "COG: %d", cog);
      ssd1306.println(dataBuffer);
      break;
    case 2:
      ssd1306.println("--- Network ---"); 
      sprintf(dataBuffer, "Net: %s", SSID);
      ssd1306.println(dataBuffer);
      sprintf(dataBuffer, "%02d:%02d:%02d UTC", hour, mins, sec);
      ssd1306.println(dataBuffer);
      break;
    default:
      break;
  }
  ssd1306.setCursor(0, 0);
  ssd1306.display();
}

void setup() {
  ssd1306.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  // initialize display
  ssd1306.display();
  delay(1000);
  ssd1306.clearDisplay();
  ssd1306.display();

  Serial.begin(115200); // Console output
  delay(100);

  // Start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);

  int count = 0;
  char dataBuffer[128];

  // Testing the screen
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  // A banner
  ssd1306.setTextSize(2);
  ssd1306.setTextColor(WHITE);
  ssd1306.setCursor(0, 0);
  ssd1306.println("TCP Watch");
  ssd1306.setTextSize(1);
  ssd1306.println("by OlivSoft");
  ssd1306.setCursor(0, 0);
  ssd1306.display(); // actually display all of the above

  delay(5000);

  // Trying to connect to the server
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    ssd1306.clearDisplay();
    ssd1306.setTextColor(WHITE);
    sprintf(dataBuffer, "%d. %s...", count, SSID);
    ssd1306.setCursor(count % 28, count % 28); // 56 for a 128x64 screen
    ssd1306.println(dataBuffer); // "Connecting...");
    ssd1306.display();
    count++;
  }
  // Connected!
  ssd1306.clearDisplay();
  ssd1306.setTextColor(WHITE);
  ssd1306.setCursor(0, 0);
  ssd1306.println("Connection to");
  ssd1306.println(SSID);
  ssd1306.println("Ready");
  ssd1306.display();

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
}

/*
   The keys of the sentences returned by the REST request,
   like 'key=value'
*/
struct KEYS {
  const String BSP = "BSP";
  const String LAT = "LAT";
  const String LNG = "LNG";
  const String SOG = "SOG";
  const String COG = "COG";
  const String DATE = "DATE";
  const String YEAR = "YEAR";
  const String MONTH = "MONTH";
  const String DAY = "DAY";
  const String HOUR = "HOUR";
  const String MIN = "MIN";
  const String SEC = "SEC";
} keys;

unsigned long lastDisplay = 0;
unsigned long lastPing = 0;

const boolean withButtons = false;

void loop() {
  if (withButtons) {
    if (!digitalRead(BUTTON_A)) {
      screenIndex = 0;
      Serial.println("Button A");
    }
    if (!digitalRead(BUTTON_B)) {
      screenIndex = 1;
      Serial.println("Button B");
    }
    if (!digitalRead(BUTTON_C)) {
      screenIndex = 2;
      Serial.println("Button C");
    }
    delay(10);
    yield(); // TODO What's that?
    //ssd1306.display();
  } else {
    unsigned long time = millis();
    if (time - lastPing > 5000) {
      screenIndex += 1;
      lastPing = time;
      if (screenIndex > 2) {
        screenIndex = 0;
      }
      Serial.print("screenIndex now ");
      Serial.println(screenIndex);
    }
  }

  unsigned long time = millis();
  if (time - lastDisplay > BETWEEN_LOOPS) {

    Serial.print(">> connecting to ");
    Serial.print(HOST);
    Serial.print(":");
    Serial.println(HTTP_PORT);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    if (!client.connect(HOST, HTTP_PORT)) {
      Serial.println("!! connection failed !!");
      return;
    }

    // Now create a URI for the REST request
    String url = REST_REQUEST;
    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    sendRequest(client, "GET", url, "HTTP/1.1", HOST);
    delay(500);

    // Read all the lines of the reply from server and print them to Serial
    // Keys are BSP, LAT, LNG, SOG, COG, DATE, YEAR, MONTH, DAY, HOUR, MIN, SEC
    while (client.available()) {
      String line = client.readStringUntil('\n');
      // Serial.println(line);
      String key = getKey(line);
      if (key.length() > 0) {
        String value = getValue(line);
        if (key == keys.BSP) {
          bsp = value.toFloat();
        } else if (key == keys.LAT) {
          lat = value.toFloat();
        } else if (key == keys.LNG) {
          lng = value.toFloat();
        } else if (key == keys.SOG) {
          sog = value.toFloat();
        } else if (key == keys.DATE) {
          date = value;
        } else if (key == keys.COG) {
          cog = value.toInt();
        } else if (key == keys.YEAR) {
          year = value.toInt();
        } else if (key == keys.MONTH) {
          month = value.toInt();
        } else if (key == keys.DAY) {
          day = value.toInt();
        } else if (key == keys.HOUR) {
          hour = value.toInt();
        } else if (key == keys.MIN) {
          mins = value.toInt();
        } else if (key == keys.SEC) {
          sec = value.toInt();
        }
      }
    }
    char dataBuffer[128];
    sprintf(dataBuffer, "BSP=%f, LAT=%f, LNG=%f", bsp, lat, lng);
    Serial.println(dataBuffer);
    sprintf(dataBuffer, "SOG=%f, COG=%d", sog, cog);
    Serial.println(dataBuffer);
    Serial.print("Date=");
    Serial.println(date);
    Serial.println("<< closing connection");

    lastDisplay = millis();
  }
  repaint(); // Display data on screen
}

void sendRequest(WiFiClient client, String verb, String url, String protocol, String host) {
  String request = verb + " " + url + " " + protocol + "\r\n" +
                   "Host: " + host + "\r\n" +
                   "Connection: close\r\n" +
                   "\r\n";
  client.print(request);
}

/*
   Reads the Key in "Key=Value"
*/
String getKey(String line) {
  int separatorPosition = line.indexOf("=");
  if (separatorPosition == -1) {
    return "";
  }
  return line.substring(0, separatorPosition);
}

/*
   Reads the Value in "Key=Value"
*/
String getValue(String line) {
  int separatorPosition = line.indexOf("=");
  if (separatorPosition == -1) {
    return "";
  }
  return line.substring(separatorPosition + 1);
}

