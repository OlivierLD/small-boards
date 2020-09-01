#include <SPI.h>
#include <WiFi.h>
#include <M5StickC.h>

#include "Colors.h"

/*
   Make REST requests - It's a client for the NavServer
   ------------------
   RST Button: top right
   HOME Button: the big one with M5 written on it

   Use the HOME button to scroll through screens.

   Inspired by https://m5stack.hackster.io/Ahork/m5stickc-for-pilot-hue-daf304

   Some doc for M5.lcd at http://forum.m5stack.com/topic/41/lesson-1-1-lcd-graphics
*/

// change values below to fit your settings
//const char* SSID = "Sonic-00e0_EXT";        // your network SSID (name)
//const char* PASSWORD = "67369c7831";        // your network password
//const char* SERVER_NAME = "192.168.42.37";  // For REST requests, Nav Server
const char* SSID = "RPi-Gateway";           // your network SSID (name)
const char* PASSWORD = "raspberrypi";       // your network password
const char* SERVER_NAME = "192.168.50.10";  // For REST requests, Nav Server
// IPAddress server(192, 168, 42, 13);
const int SERVER_PORT = 5678;               // Server port

const String LAT_PREFIX = "LAT=";
const String LNG_PREFIX = "LNG=";
const String BSP_PREFIX = "BSP=";
const String SOG_PREFIX = "SOG=";
const String COG_PREFIX = "COG=";
const String DATE_PREFIX = "DATE=";
const String UTC_YEAR_PREFIX =  "YEAR=";
const String UTC_MONTH_PREFIX = "MONTH=";
const String UTC_DAY_PREFIX =   "DAY=";
const String UTC_HOURS_PREFIX = "HOUR=";
const String UTC_MINS_PREFIX =  "MIN=";
const String UTC_SECS_PREFIX =  "SEC=";

const String SOL_HOURS_PREFIX = "S_HOUR=";
const String SOL_MINS_PREFIX =  "S_MIN=";
const String SOL_SECS_PREFIX =  "S_SEC=";

const String BARO_PREFIX = "BARO=";
const String TEMP_PREFIX = "TEMP=";
const String HUM_PREFIX =  "HUM=";

const int NONE = 0;
const int NS = 1;
const int EW = 2;

int backgroundColor = M5_BLACK; // TODO More colors
int foregroundColor = M5_WHITE;

const boolean DEBUG = false;

String lat = "";
String lng = "";
String bsp = "";
String sog = "";
String cog = "";
String date = ""; // Local

String utc_year = "";
String utc_month = "";
String utc_day = "";

String utc_hours = "";
String utc_mins = "";
String utc_secs = "";

String sol_hours = "";
String sol_mins = "";
String sol_secs = "";

String hum = "";
String baro = "";
String temp = "";

const int POS_SCREEN = 0;
const int BSP_SCREEN = 1;
const int SOG_SCREEN = 2;
const int COG_SCREEN = 3;
const int DATE_SCREEN = 4;
const int UTC_DATE_SCREEN = 5;
const int SOLAR_TIME_SCREEN = 6;
const int ATM_SCREEN = 7;

const String MONTH[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

int screens[] = {
  POS_SCREEN,
  BSP_SCREEN,
  SOG_SCREEN,
  COG_SCREEN,
  DATE_SCREEN,
  UTC_DATE_SCREEN,
  SOLAR_TIME_SCREEN,
  ATM_SCREEN
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
  String display;

  // Home Button: Change active screen
  if (digitalRead(M5_BUTTON_HOME) == LOW) {
    Serial.println(">>> Home button LOW");
    // Increment screen index
    currentScreen += 1;
    int screenArraySize = (sizeof(screens) / sizeof(int)); // Size of an Array...
    if (currentScreen > (screenArraySize - 1)) {
      currentScreen = 0;
    }
    if (DEBUG) {
      Serial.println(">>> Screen index is now " + String(currentScreen) + "/" + String(screenArraySize));
    }
    delay(100);
  }

  // Reset Button: not used
  if (digitalRead(M5_BUTTON_RST) == LOW) {
    Serial.println(">>> RST button LOW");
    Serial.println(">>> Do nothing");
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
    case UTC_DATE_SCREEN:
      displayUTCDate();
      break;
    case SOLAR_TIME_SCREEN:
      displaySolarTime();
      break;
    case ATM_SCREEN:
      displayAtmData();
      break;
    default:
      break;
  }
  delay(200); // Was 1000
}

void flipColors() {
  if (foregroundColor == M5_WHITE) {
      foregroundColor = M5_ORANGE;
      backgroundColor = M5_DARK_BLUE;
  } else {
      foregroundColor = M5_WHITE;
      backgroundColor = M5_BLACK;
  }
}

void displayPos() {
  M5.Lcd.setRotation( 3 );
  M5.Lcd.fillScreen(backgroundColor);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(foregroundColor);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("Position\n" + decToSex(lat.toFloat(), NS) + "\n" + decToSex(lng.toFloat(), EW));
}

void displayAtmData() {
  M5.Lcd.setRotation( 3 );
  M5.Lcd.fillScreen(backgroundColor);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(foregroundColor);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("Baro " + baro + "\n" + "Air " + temp + " C\n" + "Hum " + hum + " %");
}

void displayBsp() {
  M5.Lcd.setRotation( 3 );
  M5.Lcd.fillScreen(backgroundColor);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(foregroundColor);
  M5.Lcd.setTextSize(3);
  M5.Lcd.print("BSP\n" + bsp + " kts");
}

void displaySog() {
  M5.Lcd.setRotation( 3 );
  M5.Lcd.fillScreen(backgroundColor);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(foregroundColor);
  M5.Lcd.setTextSize(3);
  M5.Lcd.print("SOG\n" + sog + " kts");
}

void displayCog() {
  M5.Lcd.setRotation( 3 );
  M5.Lcd.fillScreen(backgroundColor);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(foregroundColor);
  M5.Lcd.setTextSize(3);
  M5.Lcd.print("COG\n" + cog);
}

void displayDate() {
  M5.Lcd.setRotation( 3 );
  M5.Lcd.fillScreen(backgroundColor);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(foregroundColor);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("Date (local)\n" + date);
}

void displayUTCDate() {
  M5.Lcd.setRotation( 3 );
  M5.Lcd.fillScreen(backgroundColor);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(foregroundColor);
  M5.Lcd.setTextSize(2);
  String utc = " - ";
  if (utc_year.length() > 0 &&
      utc_month.length() > 0 &&
      utc_day.length() > 0 &&
      utc_hours.length() > 0 &&
      utc_mins.length() > 0 &&
      utc_secs.length() > 0) {
        utc = String(utc_year.toInt()) + "-" +
              MONTH[utc_month.toInt() - 1] + "-" +
              String(utc_day.toInt()) + "\n" +
              String(utc_hours.toInt()) + ":" +
              String(utc_mins.toInt()) + ":" +
              String(utc_secs.toInt());
  }
  M5.Lcd.print("UTC Date\n" + utc);
}

void displaySolarTime() {
  M5.Lcd.setRotation( 3 );
  M5.Lcd.fillScreen(backgroundColor);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(foregroundColor);
  M5.Lcd.setTextSize(3);
  String solar = " - ";
//  char solar[16];
  if (sol_hours.length() > 0 &&
      sol_mins.length() > 0 &&
      sol_secs.length() > 0) {
        solar = String(sol_hours.toInt()) + ":" +
                String(sol_mins.toInt()) + ":" +
                String(sol_secs.toInt());
//        sprintf(solar, "%02d:%02d:%02d", sol_hours.toInt(), sol_mins.toInt(), sol_secs.toInt());
//  } else {
//    sprintf(solar, " - ");
  }
  M5.Lcd.print("Solar\n" + solar);
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
BARO=1013.6
TEMP=23.4
HUM=50.2
 */
void getData() {
  Serial.println("\nMaking request...");
  String cache = makeRequest("GET", "/mux/cache?option=txt");
  Serial.println("Request came back:");
  Serial.println("-----------");
  Serial.println(cache);
  Serial.println("-----------");
  flipColors();
  if (DEBUG) {
    Serial.println(cache);
  }

  lat =  extractFromCache(cache, LAT_PREFIX);
  lng =  extractFromCache(cache, LNG_PREFIX);
  bsp =  extractFromCache(cache, BSP_PREFIX);
  sog =  extractFromCache(cache, SOG_PREFIX);
  cog =  extractFromCache(cache, COG_PREFIX);
  date = extractFromCache(cache, DATE_PREFIX); // TODO Break on Date / Time

  utc_year = extractFromCache(cache, UTC_YEAR_PREFIX);
  utc_month = extractFromCache(cache, UTC_MONTH_PREFIX);
  utc_day = extractFromCache(cache, UTC_DAY_PREFIX);

  utc_hours = extractFromCache(cache, UTC_HOURS_PREFIX);
  utc_mins = extractFromCache(cache, UTC_MINS_PREFIX);
  utc_secs = extractFromCache(cache, UTC_SECS_PREFIX);

  sol_hours = extractFromCache(cache, SOL_HOURS_PREFIX);
  sol_mins = extractFromCache(cache, SOL_MINS_PREFIX);
  sol_secs = extractFromCache(cache, SOL_SECS_PREFIX);

  baro = extractFromCache(cache, BARO_PREFIX);
  temp = extractFromCache(cache, TEMP_PREFIX);
  hum = extractFromCache(cache, HUM_PREFIX);
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
