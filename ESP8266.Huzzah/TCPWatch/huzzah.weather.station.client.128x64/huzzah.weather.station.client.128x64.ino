/**
  Simple HTTP get webclient REST test
  for Huzzah/ESP8266 (no Feather)

  Main class of what will possibly be the TCP watch.

  Sends REST requests to the NavServer to get navigation data (see REST_REQUEST variable)
  That one spits out data on the Serial console, and on an oled screen SSD1306 128x64.

  Warning: Power Supply for the Huzzah can be an issue.

  @author Olivier LeDiouris

  Keywords: ESP8266, Huzzah, Specific SSD1306 management

  TODO Cleanup (Graphical primitives), deprecate (use Adafruit libs)
*/
#include <Wire.h>
#include "ssd1306_i2c.h"
#include <ESP8266WiFi.h>

#include <ArduinoJson.h> // Warning! Use 5.13.3, see https://arduinojson.org/

// #define DEBUG // Uncomment for more Serial output.

/* ----- Customizable Data ----- */
// Network and Host Names definitions, customize if necessary
#define _SSID "Sonic-00e0"
#define _PASSWORD "67369c7831"
#define _HOST "donpedro.lediouris.net"
#define _HTTP_PORT 80

const char* SSID     = _SSID;
const char* PASSWORD = _PASSWORD;

const char* HOST = _HOST;
const int HTTP_PORT = _HTTP_PORT;

const char* REST_REQUEST = "/php/weather/reports.v2/json.data.php?type=ALL&period=LAST";

const int BETWEEN_LOOPS = 10000; // in milli-sec.
/* ----- End of Customizable Data ----- */

// SSD1306 OLED Display connections and wiring
#define SDA 14
#define SCL 12

const int I2C = 0x3D; // Adafruit SSD1306 128x64

//#define RST 2

SSD1306 ssd1306(I2C, SDA, SCL);

DynamicJsonBuffer  jsonBuffer(200);
// Data from REST server
char datetime[64];
String dataDate = "";
String dataTime = "xx:xx:xx";
int wdir;
float gust, ws, rain, press, atemp, hum, dew;

int restStatus = 0;

char dataBuffer[128];

const int NS = 1;
const int EW = 2;

char* toDegMin(float data, int type) {
  int sign = (data >= 0) ? 1 : -1;
  float absData = data >= 0 ? data : -data; // abs returns an int...
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

int ping = 0;
char spin[] = { '/', '-', '\\', '|' };

void repaint(int x, int y) {
  ssd1306.clear();
  ssd1306.setColor(BLACK);
  ssd1306.fillRect(1, 1, 128, 64);
  ssd1306.setColor(WHITE);
  ssd1306.setFontScale2x2(false);

  char dataBuffer[128];
  int yOffset = 8;

  //sprintf(dataBuffer, "Wind Dir=%d, Wind Speed=%f, Temp=%f, DateTime=%s", wdir, ws, atemp, datetime);
  // press = data[0]["press"];
  // atemp = data[0]["atemp"];
  // hum = data[0]["hum"];
  sprintf(dataBuffer, "Net %s", SSID);
  ssd1306.drawString(1 + x, 0, dataBuffer);
  ssd1306.drawString(1 + x, yOffset + y, "- Weather Data -"); // Max 16 characters
  yOffset += 8;
  // TWS
  sprintf(dataBuffer, "TWS %.2f kts", ws);
  ssd1306.drawString(1 + x, yOffset + y, dataBuffer);
  yOffset += 8;
  // TWD
  sprintf(dataBuffer, "TWD %d", wdir);
  ssd1306.drawString(1 + x, yOffset + y, dataBuffer);
  yOffset += 8;
  // Air Temp
  sprintf(dataBuffer, "TEMP %.2f C", atemp);
  ssd1306.drawString(1 + x, yOffset + y, dataBuffer);
  yOffset += 8;
  // Pressure
  sprintf(dataBuffer, "PRMSL %.1f hPa", press);
  ssd1306.drawString(1 + x, yOffset + y, dataBuffer);
  yOffset += 8;
  // Rel Humidity
  sprintf(dataBuffer, "HUM %.2f %%", hum);
  ssd1306.drawString(1 + x, yOffset + y, dataBuffer);
  yOffset += 8;

  char _time[16];
  dataTime.toCharArray(_time, dataTime.length());
  sprintf(dataBuffer, "- %s %d -", _time, restStatus); // Data time and REST Status
  //                  "- XX:XX:XX 200 -"
  ssd1306.drawString(1 + x, yOffset + y, dataBuffer);

  ssd1306.display();
  if (ping >= 4) {
    ping = 0;
  }
}

void setup() {
  // initialize display
  ssd1306.init();
  ssd1306.flipScreenVertically();

  ssd1306.clear();
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
  ssd1306.clear();
  ssd1306.setColor(BLACK);
  ssd1306.fillRect(1, 1, 128, 64);
  ssd1306.setColor(WHITE);
  ssd1306.setFontScale2x2(false);
#ifdef LINE_TEST
  for (int line = 0; line < 8; line++) {
    sprintf(dataBuffer, "Line #%d. For tests", (line + 1));
    ssd1306.drawString(0, line * 8, dataBuffer);
  }
#endif

#ifdef LINE_TEST_2
  ssd1306.drawString(0, 8, "Test:");
  sprintf(dataBuffer, "L:%s", toDegMin(37.7489, NS));
  ssd1306.drawString(0, 16, dataBuffer);
  sprintf(dataBuffer, "G:%s", toDegMin(-122.5070, EW));
  ssd1306.drawString(0, 24, dataBuffer);
#endif
  // A banner, like TCP Smart watch, double size characters.
#ifndef LINE_TEST
#ifndef LINE_TEST_2
  ssd1306.setFontScale2x2(true);
  ssd1306.drawString(0,  0, "Weather");
  ssd1306.drawString(0, 16, " Station");
  ssd1306.drawString(0, 32, "  Client...");
  ssd1306.setFontScale2x2(false);
  ssd1306.drawString(0, 50, "By OlivSoft");
#endif
#endif
  ssd1306.display();
  delay(5000);

  // Trying to connect to the server
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    ssd1306.clear();
    ssd1306.setColor(BLACK);
    ssd1306.fillRect(1, 1, 128, 64);
    ssd1306.setColor(WHITE);
    ssd1306.setFontScale2x2(false);
    sprintf(dataBuffer, "%d. %s...", count, SSID);
    ssd1306.drawString(count % 56, (count % 56)/* + 8*/, dataBuffer); // "Connecting...");
    ssd1306.display();
    count++;
  }
  // Connected!
  ssd1306.clear();
  ssd1306.setColor(BLACK);
  ssd1306.fillRect(1, 1, 128, 64);
  ssd1306.setColor(WHITE);
  ssd1306.setFontScale2x2(false);
  ssd1306.drawString(1, 8, "Connection to");
  ssd1306.drawString(1, 16, SSID);
  ssd1306.drawString(1, 24, "Ready");
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

void loop() {
  delay(BETWEEN_LOOPS);

  Serial.print(">> connecting to ");
  Serial.print(HOST);
  Serial.print(":");
  Serial.println(HTTP_PORT);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(HOST, HTTP_PORT)) {
    Serial.println("!! connection failed");
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
  while (client.available()) {
    String line = client.readStringUntil('\n');
    if (line.startsWith("HTTP/1.1 ")) {
      restStatus = line.substring(9).toInt();
    }
#ifdef DEBUG
    Serial.println(line);
#endif
    JsonObject& root = jsonBuffer.parseObject(line);
    // Test if parsing succeeds.
    if (!root.success()) {
#ifdef DEBUG
      sprintlf(dataBuffer, "JSON Parse failed for [%s]", line);
      Serial.println(dataBuffer);
      // return;
#endif
    } else {
#ifdef DEBUG
      Serial.println(">> JSON Parsing OK!");
#endif
      JsonArray& data = root["data"];
      /*
          Keys are:
            time, wdir, gust, ws, rain, press, atemp, hum, dew
      */
      wdir = data[0]["wdir"];
      ws = data[0]["ws"];
      const char * _datetime = data[0]["time"]; // Must be a const. Format is time": "2018-12-26 15:37:25"
      strcpy(datetime, _datetime);
      String dt = String(datetime); // Make it a String
      dataDate = dt.substring(0, dt.indexOf(" "));
      dataTime = dt.substring(dt.indexOf(" ") + 1);
      gust = data[0]["gust"];
      rain = data[0]["rain"];
      press = data[0]["press"];
      atemp = data[0]["atemp"];
      hum = data[0]["hum"];
      dew = data[0]["dew"];
    }
  }
  // Last line (payload) is returned, only. Headers a read, but not returned.
  Serial.println();
  Serial.println("-- REST request response received. --");
  Serial.print("  >>> Response Status:");
  Serial.println(restStatus);

  sprintf(dataBuffer, "Wind Dir=%d, Wind Speed=%f, Temp=%f, DateTime=%s", wdir, ws, atemp, datetime);
  Serial.println(dataBuffer);
  Serial.println("<< closing connection");

  repaint(0, 0); // Display data on screen
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
