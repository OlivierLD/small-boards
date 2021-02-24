#include <SPI.h>
#include <WiFi.h>
#include <M5StickC.h>

/*
   Just a basic HTTP GET test.
   Make HTTP requests to wifitest.adafruit.com.

   Do display the Serial Monitor when running this one.
   ------------------
   RST Button: top right

   All displays are rotated with M5.Lcd.setRotation(ROT_180);
   Change that if you're left-handed or right-handed...

   M5.lcd apis: https://github.com/m5stack/m5-docs/blob/master/docs/en/api/lcd.md
*/

// change values below to fit your settings
const char* SSID        = "Sonic-00e0_EXT";         // your network SSID (name)
const char* PASSWORD    = "67369c7831";             // your network password
const char* SERVER_NAME = "wifitest.adafruit.com";  // For REST requests
const int SERVER_PORT   = 80;

#define ROT_0   1
#define ROT_90  2
#define ROT_180 3
#define ROT_270 4

int status = WL_IDLE_STATUS;

WiFiClient client;

void setup() {
  M5.begin();
  Serial.begin(9600);

  M5.Lcd.setRotation(ROT_180);

  status = WiFi.begin(SSID, PASSWORD);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.printf(".");
    Serial.print(".");
    delay(500);
  }

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.printf(" HttpClient2 Connected to wifi");
  Serial.println("\nHttpClient2 Connected to wifi");

  pinMode(M5_BUTTON_RST, INPUT);

  M5.Lcd.printf("\n\n HttpClient2 Press Reset Button to make a request.");
  Serial.println("\nHttpClient2 Press Reset Button to make a request.");
}

void makeRequest(String verb, String request) {
  // if you get a connection, report back via serial:
  Serial.println("Connecting...");
  if (client.connect(SERVER_NAME, SERVER_PORT)) {
    Serial.println("Connected!");
    // Make a HTTP/REST reques, cannot be more explicit! :
    String restRequest =
      verb + " " + request + " HTTP/1.1\r\n" +
      "Host: " + SERVER_NAME + ":" + String(SERVER_PORT) + "\r\n" +
      "Connection: close\r\n" +
      "\r\n"; //  + body + "\r\n";
    Serial.println(restRequest);
    client.print(restRequest);
  } else {
    Serial.println("nHttpClient2: Not connected...");
  }

  delay(500);

  boolean inBody = false;
  String content = "";
  while (client.available()) { // Response?
    Serial.println("\tReading response");
    String line = client.readStringUntil('\n');
    if (inBody) {
      content += (line + " ");
    }
    Serial.print("["); Serial.print(line + "]\t("); Serial.print(line.length()); Serial.println(" byte(s))"); // debug
    if (line.length() == 1) {
      inBody = true;
    }
  }
  Serial.println("\tDone reading response");
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println("\tStopping client");
    client.stop();
  }
  Serial.println("\tDone making request");

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(1);
  String mess = " Done reading response\n See the Serial Monitor..."; 
  if (content.length() > 0) {
    content.trim();
    mess += ("\n\n\"" + content + "\"");
  }
  M5.Lcd.print(mess);
}

void loop() {

  String display;
  // change active lamp
  if (digitalRead(M5_BUTTON_RST) == LOW) {
    Serial.println("RST button LOW");

    display = " Making HTTP request...";

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 10);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(1);
    M5.Lcd.print(display);

    makeRequest("GET", "/testwifi/index.html");
    delay(100);
  }

  delay(200);
}
