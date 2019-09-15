#include <SPI.h>
#include <WiFi.h>
#include <M5StickC.h>

/*
   Make REST requests
   ------------------
   RST Button: top right
   HOME Button: the big one with M5 on it

   A Client for a NavServer
   Use the HOME button to scroll through screens.

   Inspired by https://m5stack.hackster.io/Ahork/m5stickc-for-pilot-hue-daf304
*/

// change values below to fit your settings
const char* SSID = "Sonic-00e0_EXT";        // your network SSID (name)
const char* PASSWORD = "67369c7831";        // your network password
const char * SERVER_NAME = "192.168.42.13"; // For REST requests, Nav Server
// IPAddress server(192, 168, 42, 13);
const int SERVER_PORT = 5678;               // Server port

const String LAT_PREFIX = "LAT=";
const String LNG_PREFIX = "LNG=";
const String BSP_PREFIX = "BSP=";
const String SOG_PREFIX = "SOG=";
const String COG_PREFIX = "COG=";
const String DATE_PREFIX = "DATE=";

const int NONE = 0;
const int NS = 1;
const int EW = 2;

const boolean DEBUG = false;

String lat = "";
String lng = "";
String bsp = "";
String sog = "";
String cog = "";
String date = "";

const int POS_SCREEN = 0;
const int BSP_SCREEN = 1;
const int SOG_SCREEN = 2;
const int COG_SCREEN = 3;
const int DATE_SCREEN = 4;

int screens[] = {
  POS_SCREEN,
  BSP_SCREEN,
  SOG_SCREEN,
  COG_SCREEN,
  DATE_SCREEN
};
int currentScreen = POS_SCREEN;

int status = WL_IDLE_STATUS;

WiFiClient client;

void setup() {
  M5.begin();
  Serial.begin(9600);

  status = WiFi.begin(SSID, PASSWORD);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.printf(".");
    Serial.print(".");
  }

  M5.Lcd.printf("Connected to wifi");
  Serial.println("\nConnected to wifi");

  pinMode(M5_BUTTON_HOME, INPUT);
  pinMode(M5_BUTTON_RST, INPUT);
}

void loop() {

  getData();
  String display ;
  // change active screen
  if (digitalRead(M5_BUTTON_HOME) == LOW) {
    Serial.println("Home button LOW");
    // Increment screen index
    currentScreen += 1;
    if (currentScreen > ((sizeof(screens) / sizeof(int)) - 1)) {
      currentScreen = 0;
    }
    if (DEBUG) {
      Serial.println("Screen index is now " + String(currentScreen) + "/" + String((sizeof(screens) / sizeof(int))));
    }
    delay(100);
  }

  if (digitalRead(M5_BUTTON_RST) == LOW) {
    Serial.println("RST button LOW");
    Serial.println("Do nothing");
  }
  switch (currentScreen) {
    case POS_SCREEN:
      displayPos();
      break;
    case BSP_SCREEN:
      displayBsp();
      break;
    case SOG_SCREEN:
      displaySog();
      break;
    case COG_SCREEN:
      displayCog();
      break;
    case DATE_SCREEN:
      displayDate();
      break;
    default:
      break;
  }
  delay(1000);
}

void displayPos() {
  M5.Lcd.setRotation( 3 );
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("Position\n" + decToSex(lat.toFloat(), NS) + "\n" + decToSex(lng.toFloat(), EW));
}

void displayBsp() {
  M5.Lcd.setRotation( 3 );
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(3);
  M5.Lcd.print("BSP\n" + bsp + " kts");
}

void displaySog() {
  M5.Lcd.setRotation( 3 );
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(3);
  M5.Lcd.print("SOG\n" + sog + " kts");
}

void displayCog() {
  M5.Lcd.setRotation( 3 );
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(3);
  M5.Lcd.print("COG\n" + cog);
}

void displayDate() {
  M5.Lcd.setRotation( 3 );
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("Date\n" + date);
}

String extractFromCache(String cache, String prefix) {
  String data = "";
  int dataPos = cache.indexOf(prefix);
  if (dataPos != -1) {
    int nextCR = cache.indexOf("\n", dataPos);
    if (nextCR == -1) {
      data = cache.substring(dataPos + prefix.length());
    } else {
      data = cache.substring(dataPos + prefix.length(), nextCR);
    }
    if (DEBUG) {
      Serial.println("\t" + prefix + data);
    }
  } else {
    Serial.println("No " + prefix);
  }
  return data;
}
/*
 * Data is like that:
 *
BSP=0.00
LAT=37.748850
LNG=-122.506937
SOG=0.33
COG=306
DATE=Sep 14, 2019 5:46:03 PM
YEAR=2019
MONTH=9
DAY=15
HOUR=0
MIN=46
SEC=3
S_HOUR=16
S_MIN=40
S_SEC=27
RMC_OK=OK
 */
void getData() {
  String cache = makeRequest("GET", "/mux/cache?option=txt");
  if (DEBUG) {
    Serial.println(cache);
  }

  lat =  extractFromCache(cache, LAT_PREFIX);
  lng =  extractFromCache(cache, LNG_PREFIX);
  bsp =  extractFromCache(cache, BSP_PREFIX);
  sog =  extractFromCache(cache, SOG_PREFIX);
  cog =  extractFromCache(cache, COG_PREFIX);
  date = extractFromCache(cache, DATE_PREFIX);
}

String decToSex(double val, int type) {
  String s = "";
  double absVal = val >= 0 ? val : -val;
  // double intValue = floor(absVal);
  int i = (int) absVal;
  double dec = absVal - i;
  dec *= 60;
  if (val < 0) {
    if (type == NONE) {
      s += "- ";
    } else if (type == NS) {
      s += "S ";
    } else if (type == EW) {
      s += "W ";
    }
  } else {
    if (type == NONE) {
      s += "- ";
    } else if (type == NS) {
      s += "N ";
    } else if (type == EW) {
      s += "E ";
    }
  }
  s += (String(i) + " "); // "°");
  char decimal[16];
  sprintf(decimal, "%2.2f'", dec);
  s += decimal;
  if (DEBUG) {
    Serial.println("Sexagesimal for:" + String(val) + ", " + s + ", dec=" + String(dec));
  }
  return s;
}

String makeRequest(String verb, String request) {
  if (DEBUG) {
    Serial.println("\tConnecting...");
  }
  if (client.connect(SERVER_NAME, SERVER_PORT)) {
    if (DEBUG) {
      Serial.println("\tConnected!");
    }
    // Make a HTTP/REST request:
    String restRequest =
              verb + " " + request + " HTTP/1.1\r\n" +
              "Host: " + SERVER_NAME + ":" + String(SERVER_PORT) + "\r\n" +
              "Connection: close\r\n" +
              // "Accept: text/plain;charset=UTF-8\r\n" +
              "\r\n"; //  + body + "\r\n";
    if (DEBUG) {
      Serial.println(restRequest);
    }
    client.print(restRequest);
  } else {
    Serial.println("Not connected...");
  }

  delay(500);

  String response = "";
  while (client.available()) { // Response?
    if (DEBUG) {
      Serial.println("\tReading response");
    }
    String line = client.readStringUntil('\n');
    if (DEBUG) {
      Serial.print(line); // debug
    }
    response = response + "\n" + line;
  }
  if (DEBUG) {
    Serial.println("\tDone reading response");
  }
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    if (DEBUG) {
      Serial.println("\tStopping client");
    }
    client.stop();
  }
  if (DEBUG) {
    Serial.println("\tDone making request");
  }

  return response;
}
