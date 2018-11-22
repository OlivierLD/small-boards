/**
   Simple HTTP webclient REST, for the NavServer,
   for Feather-Huzzah/ESP8266.

   Weather Station Client, display the data

   Connects to the Network (Sonic-00e00) and pings the server.
   Server REST URL: http://donpedro.lediouris.net/php/weather/reports.v2/json.data.php?type=ALL&period=LAST

   Data can be seen at http://donpedro.lediouris.net/php/weather/reports.v2/weather.report.html

   @author Olivier LeDiouris
*/
#include <Wire.h>
#include <ESP8266WiFi.h>
// 2 libs below, available through the library manager.
// Graphical primitives doc at https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <ArduinoJson.h> // Warning! Use 5.13.3, see https://arduinojson.org/

#define ROT_0   0
#define ROT_90  1
#define ROT_180 2
#define ROT_270 3

#define NULL 0
// #define DEBUG // Uncomment for more Serial output.

#define _SSID "Sonic-00e0"
#define _PASSWORD "67369c7831"
#define _HOST "donpedro.lediouris.net"
#define _HTTP_PORT 80

const char* SSID     = _SSID;
const char* PASSWORD = _PASSWORD;

const char* HOST = _HOST;
const int HTTP_PORT = _HTTP_PORT;

const char* REST_REQUEST = "/php/weather/reports.v2/json.data.php?type=ALL&period=LAST"; // json.

const int BETWEEN_LOOPS = 500; // in milli-sec.

const int SSD1306_WIDTH = 128;
const int SSD1306_HEIGHT = 64;
const int SSD1306_I2C_ADDR = 0x3D; // for 128x64

Adafruit_SSD1306 ssd1306 = Adafruit_SSD1306(SSD1306_WIDTH, SSD1306_HEIGHT, &Wire);

DynamicJsonBuffer  jsonBuffer(200);

// Data from REST server
char * timeStr;
int wdir;
float gust, ws, rain, press, atemp, hum, dew;

float prevLat = -1000,
      prevLng = -1000;

void repaint() {
  ssd1306.clearDisplay();

  ssd1306.setTextColor(WHITE);
  ssd1306.setTextSize(1);

  char dataBuffer[128];

  ssd1306.setCursor(0, 0);
  ssd1306.setTextSize(1);
  ssd1306.println("TWS (kts)");
  ssd1306.drawFastHLine(0, 10, ssd1306.width() - 1, WHITE);
  ssd1306.setTextSize(2);
  ssd1306.setCursor(0, 14);
  sprintf(dataBuffer, "%.2f", ws);
  ssd1306.println(dataBuffer);

  ssd1306.drawFastHLine(0, 32, ssd1306.width() - 1, WHITE);
  ssd1306.setCursor(0, 35);
  ssd1306.setTextSize(1);
  ssd1306.println("TWD (deg)");
  ssd1306.drawFastHLine(0, 44, ssd1306.width() - 1, WHITE);
  ssd1306.setTextSize(2);
  ssd1306.setCursor(0, 48);
  sprintf(dataBuffer, "%03d", wdir);
  ssd1306.println(dataBuffer);

  ssd1306.drawFastHLine(0, 64, ssd1306.width() - 1, WHITE);
  ssd1306.setCursor(0, 67);
  ssd1306.setTextSize(1);
  ssd1306.println("AirTmp (C)");
  ssd1306.drawFastHLine(0, 76, ssd1306.width() - 1, WHITE);
  ssd1306.setTextSize(2);
  ssd1306.setCursor(0, 80);
  sprintf(dataBuffer, "%.2f", atemp);
  ssd1306.println(dataBuffer);

  ssd1306.setCursor(0, 0);
  ssd1306.display();
}

void setup() {
  ssd1306.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDR);
  // initialize display
  ssd1306.setRotation(ROT_90);
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

  // A banner
  ssd1306.setTextSize(1);
  ssd1306.setTextWrap(true);
  ssd1306.setTextColor(WHITE);
  ssd1306.setCursor(0, 0);
  ssd1306.println("Weather");
  ssd1306.println(" Station");
  ssd1306.println("  Client");
  ssd1306.setCursor(0, 0);
  ssd1306.display(); // actually display all of the above

  delay(5000); // Wait, to read the banner

  // Trying to connect to the server
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    ssd1306.clearDisplay();
    ssd1306.setTextColor(WHITE);
    sprintf(dataBuffer, "%d. %s...", count, SSID);
    ssd1306.setCursor(count % 56, count % 56); // 56 for a 128x64 screen, 28 for 128x32
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

unsigned long lastDisplay = 0;
char dataBuffer[128];

void loop() {
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
    sendRESTRequest(client, "GET", url, "HTTP/1.1", HOST, NULL, 0, String());
    delay(500);

    // Read all the lines of the reply from server and print them to Serial
    // Keys are BSP, LAT, LNG, SOG, COG, DATE, YEAR, MONTH, DAY, HOUR, MIN, SEC
    while (client.available()) {
      String line = client.readStringUntil('\n');
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
        Serial.println(">> Parsing OK!");
#endif        
        JsonArray& data = root["data"];
        /* 
         *  Keys are:
         *    time, wdir, gust, ws, rain, press, atemp, hum, dew
         */
        wdir = data[0]["wdir"];
        ws = data[0]["ws"];
     // timeStr = data[0]["time"];
        gust = data[0]["gust"];
        rain = data[0]["rain"];
        press = data[0]["press"];
        atemp = data[0]["atemp"];
        hum = data[0]["hum"];
        dew = data[0]["dew"];
      }
    }
    sprintf(dataBuffer, "Wind Dir=%d, Wind Speed=%f, Temp=%f", wdir, ws, atemp);
    Serial.println(dataBuffer);
    Serial.println("<< closing connection");

    lastDisplay = millis();
  }
  repaint(); // Display data on screen
}

void sendRESTRequest(WiFiClient client, String verb, String url, String protocol, String host, String headers[], int headerLen, String payload) {
  String request = verb + " " + url + " " + protocol + "\r\n" +
                   "Host: " + host + "\r\n";
  // Headers ?
  if (headers != NULL && headerLen > 0) {
    for (int i = 0; i < headerLen; i++) {
      request = String(request + headers[i] + "\r\n");
    }
  }

  request = String(request + "Connection: close\r\n" + "\r\n");

  // Payload ?
  if (payload != NULL && payload.length() > 0) {
    request = String(request + payload);
  }
  client.print(request);
}
