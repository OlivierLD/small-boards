#include "Adafruit_Arcada.h"
Adafruit_Arcada arcada;

// float PI = 3.1415926;

/**
   Graphical Building Blocks for a Compass.
   Will be later on used to display the heading on an Adafruit CLUE
*/
void setup(void) {
  Serial.begin(9600);

  // wait for serial port to connect. Needed for native USB
  //  while (!Serial) {
  //    delay(100);
  //  }

  // Start TFT and fill black
  if (!arcada.arcadaBegin()) {
    Serial.println("Failed to begin");
    while (1);
  }
  // Start
  Serial.println("Initializing Arcada...");
  arcada.displayBegin();

  // Turn on backlight
  arcada.setBacklight(255);
  cls();
}

void loop() {

  int heading = 0;
  // Loop 0..360
  while (true) {
    for (heading = 0; heading < 360; heading++) {
      displayNeedle(-heading);
      displayHeading(heading);
      delay(1000);
    }

  }

}

// Local functions.
void cls() {
  arcada.display->fillScreen(ARCADA_BLACK);
}

float toRadians(float deg) {
  return deg / (PI / 180.0);
}

void displayNeedle(int north) {
  int centerX = arcada.display->width() / 2;
  int centerY = arcada.display->height() / 2;
  int radius = min(centerX, centerY) * 0.9;
  int needleBaseWidth = 20;

  // Rose background  
  arcada.display->fillCircle(centerX, centerY, radius, ARCADA_RED);
  arcada.display->drawCircle(centerX, centerY, radius, ARCADA_WHITE);
  // Needle points. 0,0 is the top-left
  int northTipX = centerX + (int)(radius * cos(toRadians(north)));
  int northTipY = centerY - (int)(radius * sin(toRadians(north)));
  int midBottomLeftX = centerX + (int)(needleBaseWidth * cos(toRadians(north - 90)));
  int midBottomLeftY = centerY - (int)(needleBaseWidth * sin(toRadians(north - 90)));
  int midBottomRightX = centerX + (int)(needleBaseWidth * cos(toRadians(north + 90)));
  int midBottomRightY = centerY - (int)(needleBaseWidth * sin(toRadians(north + 90)));

  int southTipX = centerX + (int)(radius * cos(toRadians(north + 180)));
  int southTipY = centerY - (int)(radius * sin(toRadians(north + 180)));

  arcada.display->fillTriangle(northTipX, northTipY, midBottomLeftX, midBottomLeftY, midBottomRightX, midBottomRightY, ARCADA_YELLOW);
  arcada.display->drawTriangle(southTipX, southTipY, midBottomLeftX, midBottomLeftY, midBottomRightX, midBottomRightY, ARCADA_YELLOW);
}

void displayHeading(int heading) {
  //  arcada.display->fillScreen(ARCADA_BLACK);
  arcada.display->setTextWrap(false);
  arcada.display->setCursor(5, 30);
  arcada.display->setTextColor(ARCADA_BLUE);
  arcada.display->setTextSize(2);
  arcada.display->print(heading);
}
