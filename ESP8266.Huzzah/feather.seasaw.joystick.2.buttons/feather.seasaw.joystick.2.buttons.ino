/*
   Home Automation REST client.
   The Home Automation server is at https://github.com/OlivierLD/raspberry-coffee/tree/master/RESTRelay
   REST POST & GET requests
   - GET the status of a relay
   - POST to turn it on or off

   Keywords: ESP8266, Color Screen (Mini TFT 160x80) SeaSaw, Full REST Client, Home Automation
   Doc at https://learn.adafruit.com/adafruit-mini-tft-featherwing?view=all

   Button A: Relay ON
   Button B: Relay OFF
*/
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h> // Hardware-specific library
#include "Adafruit_miniTFTWing.h"

#define NULL 0
#define DEBUG true
#define SIMULATING false

Adafruit_miniTFTWing ss;
#define TFT_RST    -1    // we use the seesaw for resetting to save a pin

#ifdef ESP8266
   #define TFT_CS   2
   #define TFT_DC   16
#endif
#ifdef ESP32
   #define TFT_CS   14
   #define TFT_DC   32
#endif
#ifdef TEENSYDUINO
   #define TFT_CS   8
   #define TFT_DC   3
#endif
#ifdef ARDUINO_STM32_FEATHER
   #define TFT_CS   PC5
   #define TFT_DC   PC7
#endif
#ifdef ARDUINO_NRF52832_FEATHER /* BSP 0.6.5 and higher! */
   #define TFT_CS   27
   #define TFT_DC   30
#endif

// Anything else!
#if defined (__AVR_ATmega32U4__) || defined(ARDUINO_SAMD_FEATHER_M0) || defined (__AVR_ATmega328P__) || \
    defined(ARDUINO_SAMD_ZERO) || defined(__SAMD51__) || defined(__SAM3X8E__) || defined(ARDUINO_NRF52840_FEATHER)
   #define TFT_CS   5
   #define TFT_DC   6
#endif

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Make sure the 2 lines below match YOUR network
const char* ssid     = "Sonic-00e0";
const char* password = "67369c7831";

// REST Server
const char* host = "192.168.42.15";
const int port = 9876;

boolean buttonADown = false;
boolean buttonBDown = false;

void setup()   {
  Serial.begin(115200);

  /*
  while (!Serial)  delay(10);  // Wait until serial console is opened
  */

  if (!ss.begin()) {
    Serial.println("seesaw couldn't be found!");
    while(1);
  }

  Serial.print("SeeSaw Started!\tVersion: ");
  Serial.println(ss.getVersion(), HEX);

  ss.tftReset();   // reset the display
  ss.setBacklight(0x0); // TFTWING_BACKLIGHT_ON);  // turn off the backlight
  tft.initR(INITR_MINI160x80);   // initialize a ST7735S chip, mini display
//tft.setRotation(3); // 0, 3: 180
  tft.setTextSize(2);

  // Networking
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  if (!SIMULATING) {
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Netmask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("Gateway: ");
    Serial.println(WiFi.gatewayIP());
  } else {
    Serial.println("Simulating Network Connection...");
  }


  tft.initR(INITR_MINI160x80);   // initialize a ST7735S chip, mini display
  Serial.println("TFT initialized");

  tft.setRotation(1);

  tft.fillScreen(ST77XX_RED);
  delay(100);
  tft.fillScreen(ST77XX_GREEN);
  delay(100);
  tft.fillScreen(ST77XX_BLUE);
  delay(100);
  tft.fillScreen(ST77XX_BLACK);
}

char dataBuffer[128];

void loop() {
  
//tft.invertDisplay(true);

  delay(10);
  uint32_t buttons = ss.readButtons();
  //Serial.println(buttons, BIN);

  uint16_t color;

  color = ST77XX_BLACK;
  if (! (buttons & TFTWING_BUTTON_LEFT)) {
    Serial.println("LEFT");
    color = ST77XX_WHITE;
  }
  tft.fillTriangle(150, 30, 150, 50, 160, 40, color);
 
  color = ST77XX_BLACK;
  if (! (buttons & TFTWING_BUTTON_RIGHT)) {
    Serial.println("RIGHT");
    color = ST77XX_WHITE;
  }
  tft.fillTriangle(120, 30, 120, 50, 110, 40, color);

  color = ST77XX_BLACK;
  if (! (buttons & TFTWING_BUTTON_DOWN)) {
    Serial.println("DOWN");
    color = ST77XX_WHITE;
  }
  tft.fillTriangle(125, 26, 145, 26, 135, 16, color);

  color = ST77XX_BLACK;
  if (! (buttons & TFTWING_BUTTON_UP)) {
    Serial.println("UP");
    color = ST77XX_WHITE;
  }
  tft.fillTriangle(125, 53, 145, 53, 135, 63, color);

  color = ST77XX_BLACK;
  if (! (buttons & TFTWING_BUTTON_A)) {
    Serial.println("A");
    color = ST7735_GREEN;
    if (!buttonADown) { // New push
      Serial.println("Button A pushed, setting relay ON");
      tft.setRotation(3);
      tft.fillScreen(ST77XX_BLACK);
      tft.setCursor(0, 0);
      tft.setTextColor(color);
      tft.setTextWrap(true);
      tft.print("Relay ON");
      tft.setRotation(1);
      
      setRelayStatus(true);
      tft.fillScreen(ST77XX_BLACK);
    }
    buttonADown = true;
  } else {
    if (buttonADown) {
      Serial.println("Button A released");
    }
    buttonADown = false;
  }
  tft.fillCircle(30, 57, 10, color);

  color = ST77XX_BLACK;
  if (! (buttons & TFTWING_BUTTON_B)) {
    Serial.println("B");
    color = ST77XX_RED;
    if (!buttonBDown) { // New push
      Serial.println("Button B pushed, setting relay OFF");
      tft.setRotation(3);
      tft.fillScreen(ST77XX_BLACK);
      tft.setCursor(0, 0);
      tft.setTextColor(color);
      tft.setTextWrap(true);
      tft.print("Relay OFF");
      tft.setRotation(1);
      
      setRelayStatus(false);
      tft.fillScreen(ST77XX_BLACK);
    }
    buttonBDown = true;
  } else {
    if (buttonBDown) {
      Serial.println("Button B released");
    }
    buttonBDown = false;
  }
  tft.fillCircle(30, 18, 10, color);

  color = ST77XX_BLACK;
  if (! (buttons & TFTWING_BUTTON_SELECT)) {
    Serial.println("SELECT");
    color = ST77XX_WHITE;
  }
  tft.fillCircle(80, 40, 7, color);
}

void getRelayStatus() {
  String status = makeRESTRequest("/relay/status/1", "GET", (String)NULL);
  Serial.println(status);
}

void setRelayStatus(boolean state) {
  sprintf(dataBuffer, "{ status: %s }", (state ? "true" : "false"));
  String status = makeRESTRequest("/relay/status/1", "POST", dataBuffer);
  Serial.println(status);
}

String makeRESTRequest(String url, String verb, String payload) {
  Serial.println("Making REST request");
  String response;

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = port;
  if (!SIMULATING && !client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return "{\"error\":\"Connection failed\"}";
  }

  if (payload != NULL) {
    sprintf(dataBuffer, "Content.length: %d", payload.length());
    String headers[] = {
      "Content-Type: application/json",
      dataBuffer
    };
    sendRESTRequest(client, verb, url, "HTTP/1.1", host, headers, 2, payload);
  } else {
    sendRESTRequest(client, verb, url, "HTTP/1.1", host, NULL, 0, payload);
  }
  delay(250);

  int status = 0;
  if (!SIMULATING) {
    // Read all the lines of the reply from server and print them to Serial
    Serial.println("-- Receiving REST request response... --");
    while (client.available()) {
      response = client.readStringUntil('\r');
      Serial.print(response); // Output here
      if (response.startsWith("HTTP/1.1 ")) {
        status = response.substring(9).toInt();  
      }
    }
    // Last line (payload) is returned, only. Headers a read, but not returned.
    Serial.println();
    Serial.println("-- REST request response received. --");
    Serial.print("  >>> Response Status:");
    Serial.println(status);
  } else {
    Serial.println("... Simulated response");
    response = "{simulated: true}";
  }
  Serial.println();
  Serial.println("closing connection");
  Serial.println("-- REST request completed --");
  return response;
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
  } else if (DEBUG) {
    Serial.println("No Payload");
  }
  if (DEBUG) {
    Serial.println("--------------------");
    Serial.println(request);
    Serial.println("--------------------");
  }
  if (!SIMULATING) {
    client.print(request);
  }
}

