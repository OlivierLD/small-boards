#include <SPI.h>
#include <WiFi.h>
#include <M5StickC.h>

#include "Colors.h"

const boolean DEBUG = true;  // Display messages in the Serial Monitor (@9600) if set to true.

/*
   Make REST requests - It's a client for the NavServer
   ------------------
   NavServer, see https://github.com/OlivierLD/raspberry-coffee/tree/master/NMEA-mux-WebUI/small-server-extended
   ------------------

   Adjust the network name (SSID), IP address, and port.

   If DEBUG = true, Serial console at 9600 bps.

   RST Button: top right
   HOME Button: the big one with M5 written on it

   Use the HOME (M5) button to scroll through screens.

   >> Depending on the server config, you could modify the "const int SCREENS[]" array.

   Inspired by https://m5stack.hackster.io/Ahork/m5stickc-for-pilot-hue-daf304

   Some doc for M5.lcd at http://forum.m5stack.com/topic/41/lesson-1-1-lcd-graphics and
   https://github.com/m5stack/m5-docs/blob/master/docs/en/api/lcd.md

   Background blinks when data are received.
*/

const int ROT_0   = 0; // Default
const int ROT_90  = 1;
const int ROT_180 = 2;
const int ROT_270 = 3;
// 4 to 7: reverse and rotate.

const int PREFERRED_ROT = ROT_270; // ROT_90 to turn it upside down.

// change values below to fit your settings

//const char* SSID = "Sonic-00e0_EXT";        // your network SSID (name)
//const char* PASSWORD = "67369cxxx31";       // your network password
//const char* SERVER_NAME = "192.168.42.37";  // For REST requests, Nav Server

// const char* SSID = "RPi-Gateway-SDR";       // your network SSID (name)
// const char* PASSWORD = "raspberrypi";       // your network password
// const char* SERVER_NAME = "192.168.50.10";  // For REST requests, Nav Server's IP address

const char* SSID = "ZeroNetwork";              // your network SSID (name)
const char* PASSWORD = "PassWord";             // your network password

const char* SERVER_NAME = "192.168.50.10";     // For REST requests, Nav Server's IP address

// IPAddress server(192, 168, 42, 13);
const int SERVER_PORT = 9999;           // 8888;               // REST Server port

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

// Available displays
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

// Actually used screens (comment unwanted ones)
const int SCREENS[] = {
  POS_SCREEN,
  BSP_SCREEN,
  SOG_SCREEN,
  COG_SCREEN,
  DATE_SCREEN,
  UTC_DATE_SCREEN,
  SOLAR_TIME_SCREEN,
  ATM_SCREEN
};
int currentScreenIndex = 0;                      // First screen
int currentScreen = SCREENS[currentScreenIndex]; // POS_SCREEN; 

int status = WL_IDLE_STATUS;

WiFiClient client;

void setup() {
  M5.begin();
  Serial.begin(9600);

  status = WiFi.begin(SSID, PASSWORD);
  M5.Lcd.setRotation(PREFERRED_ROT); 
  int nbTry = 0;
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {

    if (nbTry % 200 == 0) {
      M5.Lcd.fillScreen(backgroundColor);
      M5.Lcd.setCursor(0, 10);
      M5.Lcd.setTextColor(foregroundColor);
    }
    
    if (nbTry % 100 == 0) {
      String mess = "\nConnecting to " + String(SSID); //  + "\n";
      M5.Lcd.print(mess);
      Serial.print(mess);
    }
    delay(500);
    M5.Lcd.printf(".");
    Serial.print(".");
    nbTry += 1;
    if (nbTry >= 1000) {
      nbTry = 0;
    }
    // flipColors();
  }

  M5.Lcd.printf("Connected to wifi.");
  Serial.println("\nConnected to wifi network " + String(SSID));

  pinMode(M5_BUTTON_HOME, INPUT);
  pinMode(M5_BUTTON_RST, INPUT); // for the example. Not used.
}

void loop() {

  if (DEBUG) {
    Serial.println("Getting data...");
  }
  getData();
  if (DEBUG) {
    Serial.println("Got the data!");
  }
  String display;

  // Home Button: Change active screen
  if (digitalRead(M5_BUTTON_HOME) == LOW) {
    Serial.println(">>> Home button LOW");
    // Increment screen index
    currentScreenIndex += 1;
    int screenArraySize = (sizeof(SCREENS) / sizeof(int)); // Size of an Array...
    if (currentScreenIndex > (screenArraySize - 1)) {
      currentScreenIndex = 0;
    }
    currentScreen = SCREENS[currentScreenIndex];
    if (DEBUG) {
      Serial.println(">>> Screen index is now " + String(currentScreenIndex) + "/" + String(screenArraySize));
      Serial.println(">>>     Screen is #" + String(currentScreen));
    }
    delay(100);
  }

  // Reset Button: not used
  if (digitalRead(M5_BUTTON_RST) == LOW) {
    Serial.println(">>> RST button LOW");
    Serial.println(">>> Doing nothing...");
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
//  if (foregroundColor == M5_WHITE) {
//    foregroundColor = M5_BLACK;
//    backgroundColor = M5_BLUE;
//  } else {
//    foregroundColor = M5_WHITE;
//    backgroundColor = M5_BLACK;
//  }
  if (foregroundColor == M5_WHITE) {
    foregroundColor = M5_RED;
    backgroundColor = M5_BLACK;
  } else {
    foregroundColor = M5_WHITE;
    backgroundColor = M5_BLACK;
  }
}

void displayPos() {
  M5.Lcd.setRotation(PREFERRED_ROT);
  M5.Lcd.fillScreen(backgroundColor);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(foregroundColor);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("Position\n" + decToSex(lat.toFloat(), NS) + "\n" + decToSex(lng.toFloat(), EW));
}

void displayAtmData() {
  M5.Lcd.setRotation(PREFERRED_ROT);
  M5.Lcd.fillScreen(backgroundColor);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(foregroundColor);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("Baro " + baro + "\n" + "Air " + temp + " C\n" + "Hum " + hum + " %");
}

void displayBsp() {
  M5.Lcd.setRotation(PREFERRED_ROT);
  M5.Lcd.fillScreen(backgroundColor);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(foregroundColor);
  M5.Lcd.setTextSize(3);
  M5.Lcd.print("BSP\n" + bsp + " kts");
}

void displaySog() {
  M5.Lcd.setRotation(PREFERRED_ROT);
  M5.Lcd.fillScreen(backgroundColor);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(foregroundColor);
  M5.Lcd.setTextSize(3);
  M5.Lcd.print("SOG\n" + sog + " kts");
}

void displayCog() {
  M5.Lcd.setRotation(PREFERRED_ROT);
  M5.Lcd.fillScreen(backgroundColor);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(foregroundColor);
  M5.Lcd.setTextSize(3);
  M5.Lcd.print("COG\n" + cog);
}

void displayDate() {
  String brokenDate = date;
  date.trim();
  if (date.length() > 0) {
    // Serial.print("Displaying "); Serial.println(date);
    // break at 3rd index of ' '
    int i;
    int thirdIndex = 0;
    int nbBlank = 0;
    for (i = 0; i < date.length(); i++) {
      if (date.charAt(i) == ' ') {
        nbBlank += 1;
        if (nbBlank == 3) {
          thirdIndex = i;
          break;
        }
      }
    }
    // Serial.print("3rd blank at "); Serial.println(thirdIndex);
    brokenDate.setCharAt(thirdIndex, '\n');
  }
  M5.Lcd.setRotation(PREFERRED_ROT);
  M5.Lcd.fillScreen(backgroundColor);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(foregroundColor);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("Date (local)\n" + brokenDate);
}

void displayUTCDate() {
  M5.Lcd.setRotation(PREFERRED_ROT);
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
  M5.Lcd.setRotation(PREFERRED_ROT);
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
    Serial.println("...No " + prefix + " found.");
  }
  return data;
}
/*
   Data comes back like that:

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
  if (DEBUG) {
    Serial.println("\nMaking request...");
  }
  String cache = makeRequest("GET", "/mux/cache?option=txt");
  flipColors();
  if (DEBUG) {
    Serial.println("Request's response came back:");
    Serial.println("-----------");
    Serial.println(cache);
    Serial.println("-----------");
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
  } else { // Val >= 0
    if (type == NONE) {
      s += "+ ";
    } else if (type == NS) {
      s += "N ";
    } else if (type == EW) {
      s += "E ";
    }
  }
  String sep = " ";
  // sep.setCharAt(0,  0x60); // "°");
  s += (String(i) + sep);
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
    Serial.println("NavServer: Not connected...");
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
