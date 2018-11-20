/**
   Shows how to rotate the screen content.
   Screen test for the Feather SSD1306 128x64 Only. Makes no sense to rotate the 128x32.
   
   Graphical primitives doc at https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives
   
   @author Olivier LeDiouris
*/
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ROT_0   0
#define ROT_90  1
#define ROT_180 2
#define ROT_270 3

// #define DEBUG 

const int SSD1306_WIDTH = 128;

const int SSD1306_I2C_ADDR = 0x3D; // for 128x64
const int SSD1306_HEIGHT = 64;

Adafruit_SSD1306 ssd1306 = Adafruit_SSD1306(SSD1306_WIDTH, SSD1306_HEIGHT, &Wire);

unsigned long started = 0;

char* msToTimeStr(long ms, boolean minSec) {
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
  if (!minSec) {
    sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
  } else {
    sprintf(buffer, "%02d:%02d", minutes, seconds);
  }
  return buffer;
}

void setup() {
  started = millis(); // init.
  ssd1306.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDR);
  
  // initialize display
  ssd1306.setRotation(ROT_90);
  ssd1306.display();
  
  delay(2000);
  ssd1306.clearDisplay();
  ssd1306.display();

  Serial.begin(115200); // Console output
  delay(100);

  // text display tests
  ssd1306.setTextSize(1);
  ssd1306.setTextColor(WHITE);
  ssd1306.setTextWrap(false);
  ssd1306.setCursor(0, 0);
  ssd1306.println("Let's go:");
  ssd1306.println("We are in. TxtSize=1");
  ssd1306.println("123456789012345678901234567890"); // Should be cut, as setTextWrap(false);
  ssd1306.setCursor(0, 0);
  ssd1306.display(); // actually display all of the above

  delay(5000);

  Serial.println("Setup completed");
}

int status = -1;

int yOffset = 0;
char dataBuffer[128];

unsigned long lastPing = 0;

void loop() {
  unsigned long time = millis();

#ifdef DEBUG
  Serial.print("Started:");
  Serial.print(started);
  Serial.print(", time:");
  Serial.print(time);
  Serial.print(", delta:");
  Serial.println(time - started);
#endif
  
  ssd1306.clearDisplay();
  ssd1306.setCursor(0, 0);
  ssd1306.setTextSize(2);
  ssd1306.println("Time");
  ssd1306.drawFastHLine(0, 20, SSD1306_WIDTH - 1, WHITE);
  ssd1306.setCursor(0, 24);
  sprintf(dataBuffer, "%s", msToTimeStr(time - started, true));
  ssd1306.println(dataBuffer);
  ssd1306.drawFastHLine(0, 42, SSD1306_WIDTH - 1, WHITE);
  ssd1306.setTextSize(1);
  ssd1306.setCursor(0, 46);
  sprintf(dataBuffer, "%s", msToTimeStr(time - started, false));
  ssd1306.println(dataBuffer);
  ssd1306.drawFastHLine(0, 56, SSD1306_WIDTH - 1, WHITE);

  ssd1306.setCursor(0, 118);
  ssd1306.println("Oliv did it.");

  delay(10);
  yield(); // Send in background and move on
  ssd1306.display();
}

