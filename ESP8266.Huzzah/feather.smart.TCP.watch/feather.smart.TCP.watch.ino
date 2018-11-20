/**
   Simple HTTP webclient REST, for the NavServer,
   for Feather-Huzzah/ESP8266.

   Main class of what will be the TCP watch.

   Sends REST requests to the NavServer to get navigation data (see REST_REQUEST variable)
   That one spits out data on the Serial console, and on an oled screen SSD1306 128x32.
   Ideally stacked on top of the Feather.

   @author Olivier LeDiouris

   Originally inspired by https://learn.adafruit.com/micropython-oled-watch
*/
#include <Wire.h>
#include <ESP8266WiFi.h>
// 2 libs below, available through the library manager.
// Repo at https://github.com/adafruit/Adafruit_SSD1306/blob/master/Adafruit_SSD1306.cpp and around.
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define NULL 0
// #define DEBUG // Uncomment for more Serial output.

/* ----- Customizable Data ----- */
// Network and Host Names definitions, customize if necessary
#define __PI_NET__       // 192.168.127.1 on Pi-Net
// #undef __PI_NET__

// #define __SONIC_AT_HOME__   // 192.168.42.4 on Sonic-00e0
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

char* msToTimeStr(long ms) {
  int seconds = (int)(ms / 1000);
  int minutes = 0;
  int hours = 0;
  if (seconds > 59) {
    minutes = (int)(seconds / 60);
    seconds -= (minutes * 60);
  }
  if (minutes > 59) {
    hours = (int)(minutes / 60);
    minutes -= (hours * 60);
  }
  char buffer[128];
  sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
  return buffer;
}

const int WIDTH = 128;
const int HEIGHT = 32;
const int RADIUS = 16;

void drawLine(int fromX, int fromY, int toX, int toY) {
  int deltaX = (toX - fromX);
  int deltaY = (toY - fromY);
  if (deltaX == 0 && deltaY == 0) {
    ssd1306.drawPixel(fromX, fromY, WHITE);
    return;
  }
  if (deltaX == 0) {
    for (int y = min(fromY, toY); y <= max(toY, fromY); y++) {
      if (fromX >= 0 && fromX < WIDTH && y >= 0 && y < HEIGHT) {
        ssd1306.drawPixel(fromX, y, WHITE);
      }
    }
  } else if (deltaY == 0) {
    for (int x = min(fromX, toX); x <= max(toX, fromX); x++) {
      if (x >= 0 && x < WIDTH && fromY >= 0 && fromY < HEIGHT) {
        ssd1306.drawPixel(x, fromY, WHITE);
      }
    }
  } else if (abs(deltaX) >= abs(deltaY)) { // [-45, +45]
    if (deltaX < 0) {
      int X = fromX;
      int Y = fromY;
      fromX = toX;
      toX = X;
      fromY = toY;
      toY = Y;
      deltaX = (toX - fromX);
      deltaY = (toY - fromY);
    }
    float coeffDir = (float) deltaY / (float) deltaX;
    //    if (fromX < toX)
    {
      for (int x = 0; x <= deltaX; x++) {
        int y = fromY + (int) (round(x * coeffDir));
        int _x = x + fromX;
        if (_x >= 0 && _x < WIDTH && y >= 0 && y < HEIGHT) {
          ssd1306.drawPixel(_x, y, WHITE);
        }
      }
    }
  } else if (abs(deltaX) < abs(deltaY)) { // > 45, < -45
    if (deltaY < 0) {
      int X = fromX;
      int Y = fromY;
      fromX = toX;
      toX = X;
      fromY = toY;
      toY = Y;
      deltaX = (toX - fromX);
      deltaY = (toY - fromY);
    }
    float coeffDir = (float) deltaX / (float) deltaY;
    //    if (fromX < toX)
    {
      for (int y = 0; y <= deltaY; y++) {
        int x = fromX + (int) (round(y * coeffDir));
        int _y = y + fromY;
        if (_y >= 0 && _y < HEIGHT && x >= 0 && x < WIDTH) {
          ssd1306.drawPixel(x, _y, WHITE);
        }
      }
    }
  }
}

float degToRadians(float deg) {
  return (deg * 71) / 4068;
}

void drawHeading(int heading) {
  ssd1306.clearDisplay();
  // Face
  for (int deg = 0; deg < 360; deg++) {
    float rad =  degToRadians((float)deg);
    int x = (int)(RADIUS * sin(rad));
    int y = (int)(RADIUS * cos(rad));
    ssd1306.drawPixel(16 + x, 16 + y, WHITE);
  }
  {
    float rad = degToRadians(heading);
    int x = round((RADIUS * 0.75) * sin(rad));
    int y = round((RADIUS * 0.75) * cos(rad));
    drawLine(16, 16, 16 + x, 16 - y);
  }
}

void drawWatch(int hours, int minutes, int seconds) {
  ssd1306.clearDisplay();
  // Face
  for (int deg = 0; deg < 360; deg++) {
    float rad =  degToRadians((float)deg);
    int x = (int)(RADIUS * sin(rad));
    int y = (int)(RADIUS * cos(rad));
    ssd1306.drawPixel(16 + x, 16 + y, WHITE);
  }
  float hoursInDegrees = ((hours % 12) * 30) + (((minutes * 6) + (seconds / 10)) / 12);
  float minInDegrees = ((minutes * 6) + (seconds / 10));
  float secInDegrees = seconds * 6;

  //sprintf(dataBuffer, "ms:%d => %02d:%02d:%02d, %f", ms, hours, minutes, seconds, secInDegrees);
  //Serial.println(dataBuffer);

  // Seconds
  {
    float rad = degToRadians(secInDegrees);
    int x = round((RADIUS * 0.75) * sin(rad));
    int y = round((RADIUS * 0.75) * cos(rad));
    //    if (seconds == 7 || seconds == 8 || seconds == 22 || seconds == 23 || seconds == 37 || seconds == 38 || seconds == 52 || seconds == 53) {
    //      sprintf(dataBuffer, "Sec: %d => deg: %f, rad: %f, x: %d, y: %d", seconds, secInDegrees, rad, x, y);
    //      Serial.println(dataBuffer);
    //    }
    drawLine(16, 16, 16 + x, 16 - y);
  }
  // Minutes
  {
    float rad = degToRadians(minInDegrees);
    int x = round((RADIUS * 0.65) * sin(rad));
    int y = round((RADIUS * 0.65) * cos(rad));
    drawLine(16, 16, 16 + x, 16 - y);
  }
  // Hours
  {
    float rad = degToRadians(hoursInDegrees);
    int x = round((RADIUS * 0.5) * sin(rad));
    int y = round((RADIUS * 0.5) * cos(rad));
    drawLine(16, 16, 16 + x, 16 - y);
  }
}

int screenIndex = 0;
boolean charOnly = false;

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
      ssd1306.println("----- GPS ------");
      break;
    case 1:
      if (charOnly) {
        ssd1306.println("--- Displacement ---");
        sprintf(dataBuffer, "SOG: %.2f kts", sog);
        ssd1306.println(dataBuffer);
        sprintf(dataBuffer, "COG: %d", cog);
        ssd1306.println(dataBuffer);
      } else {
        // Draw compass
        drawHeading(cog);
        // Then COG-SOG
        ssd1306.setCursor(40, 0);
        sprintf(dataBuffer, "SOG: %.2f kts", sog);
        ssd1306.println(dataBuffer);
        ssd1306.setCursor(40, 10);
        sprintf(dataBuffer, "COG: %d", cog);
        ssd1306.println(dataBuffer);
      }
      break;
    case 2:
      if (charOnly) {
        ssd1306.println("--- Network ---");
        sprintf(dataBuffer, "Net: %s", SSID);
        ssd1306.println(dataBuffer);
        sprintf(dataBuffer, "%02d:%02d:%02d UTC", hour, mins, sec);
        ssd1306.println(dataBuffer);
      } else {
        // Draw watch
        drawWatch(hour, mins, sec);
        // Then display time
        ssd1306.setCursor(40, 0);
        sprintf(dataBuffer, "%02d:%02d:%02d UTC", hour, mins, sec);
        ssd1306.println(dataBuffer);
        ssd1306.setCursor(40, 10);
        sprintf(dataBuffer, "Net:%s", SSID);
        ssd1306.println(dataBuffer);
      }
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

  // Initializing buttons
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
    yield(); // Send to backgbround, or so.
    //ssd1306.display();
  } else {
    unsigned long time = millis();
    if (time - lastPing > 5000) {
      screenIndex += 1; // Scroll to next screen
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
    sendRESTRequest(client, "GET", url, "HTTP/1.1", HOST, NULL, 0, String());
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

