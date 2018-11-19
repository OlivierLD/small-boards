/**
   Test for the Feather SSD1306 128x32
   Test with buttons
   
   @author Olivier LeDiouris
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

void setup() {
  ssd1306.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  // initialize display
  ssd1306.display();
  delay(2000);
  ssd1306.clearDisplay();
  ssd1306.display();

  Serial.begin(115200); // Console output
  delay(100);

  char dataBuffer[128];

  // Testing the screen
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  // TODO A banner, like TCP Smart watch, double size characters.
  // text display tests
  ssd1306.setTextSize(1);
  ssd1306.setTextColor(WHITE);
  ssd1306.setCursor(0, 0);
  ssd1306.println("Let's go:"); // TODO Change this
  ssd1306.println("We are in.\nText Size = 1");
  ssd1306.setCursor(0, 0);
  ssd1306.display(); // actually display all of the above

  delay(5000);

  Serial.println("Setup completed");
}

int status = 0;

int yOffset = 0;
char dataBuffer[128];

void loop() {
  if (!digitalRead(BUTTON_A)) {
    Serial.println("Button A");
    status = 1;
  }
  if (!digitalRead(BUTTON_B)) {
    Serial.println("Button B");
    yOffset += 1;
    if (yOffset > 32) { // Last full line is 24 with text size = 1, bottom truncated after that.
      yOffset = 0;
    }
    status = 2;
  }
  if (!digitalRead(BUTTON_C)) {
    Serial.println("Button C");
    status = 3;
  }

  switch (status) {
    case 1:
      ssd1306.clearDisplay();
      ssd1306.setCursor(0, 0);
      ssd1306.setTextSize(2);
      ssd1306.println("1 - TextSize 2");
      ssd1306.setTextSize(1);
      ssd1306.println("Oliv did it.");
      break;
    case 2:
      ssd1306.clearDisplay();
      ssd1306.setCursor(0, yOffset);
      ssd1306.setTextSize(1);
      sprintf(dataBuffer, "2 - yOffset = %d", yOffset);
      ssd1306.println(dataBuffer);
      break;
    case 3:
      ssd1306.clearDisplay();
      ssd1306.setCursor(0, 0);
      ssd1306.setTextSize(3);
      ssd1306.println("Size 3");
      ssd1306.setTextSize(1);
      ssd1306.println("3 - Very big, hey?");
      break;
    default:
      break;
  }

  delay(10);
  yield(); // Send in background...
  ssd1306.display();
}

