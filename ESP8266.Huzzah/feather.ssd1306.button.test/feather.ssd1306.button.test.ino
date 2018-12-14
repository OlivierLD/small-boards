/**
   Test for the Feather SSD1306 128x32 Stack oled screen
   Test with buttons. Push and Release detection.

   @author Olivier LeDiouris

   Keywords: ESP8266, Feather, SSD1306 Cap, 3 buttons. 
   Push and release management.
*/
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

boolean buttonADown = false;
boolean buttonBDown = false;
boolean buttonCDown = false;

void setup() {
  ssd1306.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  // initialize display
  ssd1306.display();
  delay(2000);
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
  ssd1306.println("Push buttons tests.\nPush and release.");
  ssd1306.setCursor(0, 0);
  ssd1306.display(); // actually display all of the above

  delay(5000);

  Serial.println("Setup completed");
}

void loop() {
  if (!digitalRead(BUTTON_A)) {
    if (!buttonADown) { // New push
      display("<- Button A is pushed", 0, 0);
      Serial.println("Button A pushed");
    }
    buttonADown = true;
  } else {
    if (buttonADown) {
      display("Button A released", 0, 0);
      Serial.println("Button A released");
    }
    buttonADown = false;
  }
  if (!digitalRead(BUTTON_B)) {
    if (!buttonBDown) { // New push
      display("<- Button B is pushed", 0, 10);
      Serial.println("Button B pushed");
    }
    buttonBDown = true;
  } else {
    if (buttonBDown) {
      display("Button B released", 0, 0);
      Serial.println("Button B released");
    }
    buttonBDown = false;
  }
  if (!digitalRead(BUTTON_C)) {
    if (!buttonCDown) { // New push
      display("<- Button C is pushed", 0, 20);
      Serial.println("Button C pushed");
    }
    buttonCDown = true;
  } else {
    if (buttonCDown) {
      display("Button C released", 0, 0);
      Serial.println("Button C released");
    }
    buttonCDown = false;
  }

  // More than 1 button pressed?
  int nbButtonDown = 0 + (buttonADown ? 1 : 0) + (buttonBDown ? 1 : 0) + (buttonCDown ? 1 : 0);
  if (nbButtonDown > 1) {
    Serial.print("Pressing ");
    if (buttonADown) {
      Serial.print("A ");  
    }
    if (buttonBDown) {
      Serial.print("B ");  
    }
    if (buttonCDown) {
      Serial.print("C ");  
    }
    Serial.println();
  }

  delay(10);
  yield(); // Send in background...
//ssd1306.display();
}

void display(String text, int x, int y) {
  ssd1306.clearDisplay();
  ssd1306.setCursor(x, y);
  ssd1306.println(text);
  ssd1306.setCursor(0, 0);
  ssd1306.display();
}

