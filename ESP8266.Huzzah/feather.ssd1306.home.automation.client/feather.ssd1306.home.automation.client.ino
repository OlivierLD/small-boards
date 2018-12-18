/*
   Home Automation REST client.
   The Home Automation server is at https://github.com/OlivierLD/raspberry-coffee/tree/master/RESTRelay
   REST POST & GET requests
   - GET the status of a relay
   - POST to turn it on or off

   Keywords: ESP8266, SSD1306, Full REST Client, Home Automation

   Required extra libraries:
   - Adafruit GFX
   - Adafruit SSD1306
   - RESTHelper (see ../RESTHElper.lib)
*/

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define NULL 0
#define DEBUG true
#define SIMULATING false

#include <RESTHelper.h>

/*
  >>>> Uncomment next line if running on a small screen !!
*/
#define SSD1306_128x32

#ifdef SSD1306_128x32

#define I2C_ADDR 0x3C
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 32

#else

#define I2C_ADDR 0x3D
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

#endif

Adafruit_SSD1306 ssd1306 = Adafruit_SSD1306(SSD1306_WIDTH, SSD1306_HEIGHT, &Wire);

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

boolean buttonADown = false;
boolean buttonBDown = false;
boolean buttonCDown = false;

// Network and Host definitions

// Make sure the 2 lines below match YOUR network
const char* ssid     = "Sonic-00e0";
const char* password = "67369c7831";

// Modify this too: hostname and port where the REST Server is running
const char* host = "192.168.42.15";
const int port = 9876;

RESTHelper restHelper(SIMULATING);

unsigned long lastDisplay = 0;
boolean menuIsDisplayed = true;

void setup() {
  ssd1306.begin(SSD1306_SWITCHCAPVCC, I2C_ADDR); // Address 0x3C for 128x32, 0x3D otherwise
  // initialize display
  ssd1306.display();
  delay(1000);
  ssd1306.clearDisplay();
  ssd1306.display();

  Serial.begin(115200); // Console output
  delay(100);

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  ssd1306.setTextSize(1);
  ssd1306.setTextColor(WHITE);
  ssd1306.setCursor(0, 0);
  ssd1306.println("Home Automation.\nPush and release.");
  ssd1306.setCursor(0, 0);
  ssd1306.display(); // actually display all of the above

  //delay(1000);

  // We start by connecting to a WiFi network
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

  lastDisplay = millis(); // init.
  displayMenu();
}

void displayMenu() {
  display("<- A - Relay status\n<- B - Relay ON\n<- C - Relay OFF", 0, 0);
  Serial.println("Displaying menu");
  menuIsDisplayed = true;
}

char dataBuffer[128];

void loop() {
  if (!digitalRead(BUTTON_A)) {
    if (!buttonADown) { // New push
      display("Requesting status", 0, 0);
      Serial.println("Button A pushed");
      getRelayStatus();
    }
    buttonADown = true;
  } else {
    if (buttonADown) {
      //    display("Button A released", 0, 0);
      Serial.println("Button A released");
      lastDisplay = millis();
    }
    buttonADown = false;
  }
  if (!digitalRead(BUTTON_B)) {
    if (!buttonBDown) { // New push
      display("Setting relay ON", 0, 10);
      Serial.println("Button B pushed");
      setRelayStatus(true);
    }
    buttonBDown = true;
  } else {
    if (buttonBDown) {
      //    display("Button B released", 0, 0);
      Serial.println("Button B released");
      lastDisplay = millis();
    }
    buttonBDown = false;
  }
  if (!digitalRead(BUTTON_C)) {
    if (!buttonCDown) { // New push
      display("Setting relay OFF", 0, 20);
      Serial.println("Button C pushed");
      setRelayStatus(false);
    }
    buttonCDown = true;
  } else {
    if (buttonCDown) {
      //    display("Button C released", 0, 0);
      Serial.println("Button C released");
      lastDisplay = millis();
    }
    buttonCDown = false;
  }
  long now = millis();
  if (!menuIsDisplayed && (now - lastDisplay) > 10000) { // Re-displaying menu after 10 sec
    displayMenu();
  }
  delay(10);
  yield(); // Send in background...
}

void getRelayStatus() {
  RESTHelper::Response statusResponse = restHelper.makeRESTRequest(host, port, "/relay/status/1", "GET", (String)NULL);
  Serial.println(statusResponse.content);
  display(statusResponse.content, 0, 0);
  lastDisplay = millis();
}

void setRelayStatus(boolean state) {
  sprintf(dataBuffer, "{ status: %s }", (state ? "true" : "false"));
  RESTHelper::Response statusResponse = restHelper.makeRESTRequest(host, port, "/relay/status/1", "POST", dataBuffer);
  Serial.println(statusResponse.content);
  display(statusResponse.content, 0, 15);
  lastDisplay = millis();
}

void display(String text, int x, int y) {
  ssd1306.clearDisplay();
  ssd1306.setCursor(x, y);
  ssd1306.println(text);
  ssd1306.setCursor(0, 0);
  ssd1306.display();
  menuIsDisplayed = false;
}


