/**
   Simple HTTP webclient REST, for the NavServer,
   for Feather-Huzzah/ESP8266.

   Main class of what will be the TCP watch.

   Sends REST requests to the NavServer to get navigation data (see REST_REQUEST variable)
   That one spits out data on the Serial console, and on an oled screen SSD1306 128x64, rotated.

   Also draws a map of the recent positions.

   @author Olivier LeDiouris
*/
#include <Wire.h>
#include <ESP8266WiFi.h>
// 2 libs below, available through the library manager.
// Repo at https://github.com/adafruit/Adafruit_SSD1306/blob/master/Adafruit_SSD1306.cpp and around.
// Graphical primitives doc at https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ROT_0   0
#define ROT_90  1
#define ROT_180 2
#define ROT_270 3

#define NULL 0
// #define DEBUG // Uncomment for more Serial output.

/* ----- Customizable Data ----- */
// Network and Host Names definitions, customize if necessary
// #define __PI_NET__       // 192.168.127.1 on Pi-Net
#undef __PI_NET__

#define __SONIC_AT_HOME__   // 192.168.42.4 on Sonic-00e0
// #undef __SONIC_AT_HOME__

#include "custom_values.h" // Contains _SSID, _PASSWORD, _HOST, _HTTP_PORT, used below.

#define POS_LIST_MAX_LENGTH 200

typedef struct POS {
  struct POS * next;
  float lat;
  float lng;
} Pos;

/* Chain Lists Utils */
typedef struct SMALL {
  struct SMALL * next;
} StrPt;

void chainAppend (StrPt * head, StrPt * pt) {
  StrPt * structPt = head;

  while (structPt->next) {
    structPt = structPt->next;
  }
  structPt->next = pt;
}

void freeChain (StrPt * head) {
  StrPt * nextOne;
  StrPt * thisOne;

  thisOne = head;
  while (thisOne) { 
    nextOne = thisOne->next;
    free((char *)thisOne);
    thisOne = nextOne;
  }
}

#define walkList(a,b) for (b=a;b;b=b->next)

void chainRemove (StrPt * head, StrPt * pt) {
  StrPt * locPt;
  walkList(head, locPt) {
    if (locPt->next == pt) {
      locPt->next = pt->next;
      break;
    }
  }
}

StrPt * dropHead (StrPt * head) {
  if (head != NULL) {
    if (head->next != NULL) {
      StrPt * newHead = head->next;
      free((char*)head);
      return newHead;
    }
  }
  return NULL;
}

int listLength(StrPt * head) {
  int len = 0;
  StrPt * locPt;
  walkList(head, locPt) {
    len += 1;
  }
  return len;
}

const char* SSID     = _SSID;
const char* PASSWORD = _PASSWORD;

const char* HOST = _HOST;
const int HTTP_PORT = _HTTP_PORT;

const char* REST_REQUEST = "/mux/cache?option=txt"; // txt, not json.

const int BETWEEN_LOOPS = 500; // in milli-sec.
/* ----- End of Customizable Data ----- */

const int SSD1306_WIDTH = 128;
const int SSD1306_HEIGHT = 64;
const int SSD1306_I2C_ADDR = 0x3D; // for 128x64

Adafruit_SSD1306 ssd1306 = Adafruit_SSD1306(SSD1306_WIDTH, SSD1306_HEIGHT, &Wire);

// Data from REST server
float bsp, lat, lng, sog;
int cog, year, month, day, hour, mins, sec;
String date;

float prevLat = -1000, 
      prevLng = -1000;

Pos * posListHead = NULL;

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

int screenIndex = -1;

void repaint() {
  ssd1306.clearDisplay();

  ssd1306.setTextColor(WHITE);
  ssd1306.setTextSize(1);

  char dataBuffer[128];

  switch (screenIndex) {
    case 0:
      ssd1306.setCursor(0, 0);
      ssd1306.setTextSize(1);
      ssd1306.println("SOG (kts)");  
      ssd1306.drawFastHLine(0, 10, ssd1306.width() - 1, WHITE);
      ssd1306.setTextSize(2);
      ssd1306.setCursor(0, 14);
      sprintf(dataBuffer, "%.2f", sog);
      ssd1306.println(dataBuffer);
      ssd1306.drawFastHLine(0, 32, ssd1306.width() - 1, WHITE);
      ssd1306.setCursor(0, 35);
      ssd1306.setTextSize(1);
      ssd1306.println("COG (deg)");  
      ssd1306.drawFastHLine(0, 44, ssd1306.width() - 1, WHITE);
      ssd1306.setTextSize(2);
      ssd1306.setCursor(0, 48);
      sprintf(dataBuffer, "%03d", cog);
      ssd1306.println(dataBuffer);
      break;
    case 1:
      // Draw a map
      ssd1306.drawRect(0, 0, ssd1306.width(), ssd1306.height(), WHITE);
      // Draw data here.
      if (posListHead != NULL) {
        // 1 - Obtain boundaries
        float minLat = 1000, maxLat = -1000, minLng = 1000, maxLng = -1000;
        Pos * it;
        walkList(posListHead, it) {
          minLat = min(it->lat, minLat);
          maxLat = max(it->lat, maxLat);
          minLng = min(it->lng, minLng);
          maxLng = max(it->lng, maxLng);
        }
        // 2 - Now plot
        float deltaLat = maxLat - minLat;
        float deltaLng = maxLng - minLng;
#ifdef DEBUG        
        sprintf(dataBuffer, "lat in [%f..%f], lng in [%f..%f]", minLat, maxLat, minLng, maxLng);
        Serial.println(dataBuffer);
#endif        
        float distCoeff = min( ((float)ssd1306.width()) / deltaLng, ((float)ssd1306.height()) / deltaLat) * 0.8; // 0.8, not to hit the borders
        // Square projection for now...
        int prevX = -1, prevY = -1;
        walkList(posListHead, it) {
          float graphLng = it->lng - minLng;
          float fromCenterLng = graphLng - (deltaLng / 2);
          int x = round(((float)ssd1306.width() / 2) + (fromCenterLng * distCoeff)); 
          float graphLat = it->lat - minLat;
          float fromCenterLat = graphLat - (deltaLat / 2);
          int y = round(((float)ssd1306.height() / 2) - (fromCenterLat * distCoeff)); 
          if (prevX == -1 && prevY == -1) {
            ssd1306.drawPixel(x, y, WHITE);
          } else {
            ssd1306.drawLine(prevX, prevY, x, y, WHITE);
          }
          prevX = x;
          prevY = y;
        }    
        // Plot last point
        if (prevX != -1 && prevY != -1) {
          ssd1306.fillCircle(prevX, prevY, 2, WHITE);
        }
      }
      // Overlay SOG-COG
      ssd1306.setCursor(2, 2);
      sprintf(dataBuffer, "SOG:%.2f", sog);
      ssd1306.println(dataBuffer);
      ssd1306.setCursor(2, 12);
      sprintf(dataBuffer, "COG:%03d", cog);
      ssd1306.println(dataBuffer);
     
      break;
    default:
      break;
  }
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
  ssd1306.setTextSize(2);
  ssd1306.setTextWrap(true);
  ssd1306.setTextColor(WHITE);
  ssd1306.setCursor(0, 0);
  ssd1306.println("TCP");
  ssd1306.println("Watch");
  ssd1306.setTextSize(1);
  ssd1306.println("by");
  ssd1306.println("OlivSoft");
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

const int NB_SCREEN = 2;
void loop() {
  unsigned long time = millis();
  if (time - lastPing > 5000) {
    screenIndex += 1; // Scroll to next screen
    lastPing = time;
    if (screenIndex > (NB_SCREEN - 1)) {
      screenIndex = 0;
    }
    Serial.print("screenIndex now ");
    Serial.println(screenIndex);
  }

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

    // New position? (for the map)
    if (lat != prevLat || lng != prevLng) {
      if (posListHead == NULL) {
        posListHead = (Pos *) calloc(1, sizeof(Pos)); 
        posListHead->next = NULL;
        posListHead->lat = lat;
        posListHead->lng = lng;
      } else {
        Pos * newPos = (Pos *) calloc(1, sizeof(Pos)); 
        newPos->next = NULL;
        newPos->lat = lat;
        newPos->lng = lng;
        chainAppend((StrPt *)posListHead, (StrPt *)newPos);
      }
      prevLat = lat;
      prevLng = lng;
      // Trim list if needed
      while (listLength((StrPt *)posListHead) > POS_LIST_MAX_LENGTH) {
        posListHead = (Pos *)dropHead((StrPt *)posListHead);
      }
    }
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

