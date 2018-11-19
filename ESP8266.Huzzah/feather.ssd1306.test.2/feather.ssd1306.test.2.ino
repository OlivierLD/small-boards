/**
   Screen test for the Feather SSD1306 128x32
   Test with NO buttons
   
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

unsigned long started = 0;

char* msToTimeStr(long ms) {
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
  sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
  return buffer;
}

char* msToTimeDisplay(long ms) {
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
  if (hours > 0) {
    sprintf(buffer, "%02d h %02d m %02d s", hours, minutes, seconds);
  } else if (minutes > 0) {
    sprintf(buffer, "%02d m %02d s", minutes, seconds);
  } else {
    sprintf(buffer, "%02d s", seconds);
  }
  return buffer;
}

const int WIDTH = 128;
const int HEIGHT = 32;
void drawLine(int fromX, int fromY, int toX, int toY) {
  //  float deltaX = (toX - fromX);
  //  float deltaY = (toY - fromY);
  //  float maxDelta = fabs(max(deltaX, deltaY));
  //  for (int i = 0; i < maxDelta; i++) {
  //    int _x = (int)(fromX + (i * (deltaX / maxDelta)));
  //    int _y = (int)(fromY + (i * (deltaY / maxDelta)));
  //    ssd1306.drawPixel(_x, _y, WHITE);
  //  }
  int deltaX = (toX - fromX);
  int deltaY = (toY - fromY);
  if (deltaX == 0 && deltaY == 0) {
    ssd1306.drawPixel(fromX, fromY, WHITE);
    return;
  }
  if (deltaX == 0) {
    for (int y = min(fromY, toY); y <= max(toY, fromY); y++) {
      if (fromX >= 0 && fromX < WIDTH && y >= 0 && y < HEIGHT) {
        ssd1306.drawPixel(fromX, y, WHITE);
      }
    }
  } else if (deltaY == 0) {
    for (int x = min(fromX, toX); x <= max(toX, fromX); x++) {
      if (x >= 0 && x < WIDTH && fromY >= 0 && fromY < HEIGHT) {
        ssd1306.drawPixel(x, fromY, WHITE);
      }
    }
  } else if (abs(deltaX) >= abs(deltaY)) { // [-45, +45]
    if (deltaX < 0) {
      int X = fromX;
      int Y = fromY;
      fromX = toX;
      toX = X;
      fromY = toY;
      toY = Y;
      deltaX = (toX - fromX);
      deltaY = (toY - fromY);
    }
    float coeffDir = (float) deltaY / (float) deltaX;
    //    if (fromX < toX)
    {
      for (int x = 0; x <= deltaX; x++) {
        int y = fromY + (int) (round(x * coeffDir));
        int _x = x + fromX;
        if (_x >= 0 && _x < WIDTH && y >= 0 && y < HEIGHT) {
          ssd1306.drawPixel(_x, y, WHITE);
        }
      }
    }
  } else if (abs(deltaX) < abs(deltaY)) { // > 45, < -45
    if (deltaY < 0) {
      int X = fromX;
      int Y = fromY;
      fromX = toX;
      toX = X;
      fromY = toY;
      toY = Y;
      deltaX = (toX - fromX);
      deltaY = (toY - fromY);
    }
    float coeffDir = (float) deltaX / (float) deltaY;
    //    if (fromX < toX)
    {
      for (int y = 0; y <= deltaY; y++) {
        int x = fromX + (int) (round(y * coeffDir));
        int _y = y + fromY;
        if (_y >= 0 && _y < HEIGHT && x >= 0 && x < WIDTH) {
          ssd1306.drawPixel(x, _y, WHITE);
        }
      }
    }
  }
}

void setup() {
  started = millis(); // init.
  ssd1306.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  // initialize display
  ssd1306.display();
  delay(2000);
  ssd1306.clearDisplay();
  ssd1306.display();

  Serial.begin(115200); // Console output
  delay(100);

  // text display tests
  ssd1306.setTextSize(1);
  ssd1306.setTextColor(WHITE);
  ssd1306.setCursor(0, 0);
  ssd1306.println("Let's go:");
  ssd1306.println("We are in. TxtSize=1");
  ssd1306.println("123456789012345678901234567890");
  ssd1306.setCursor(0, 0);
  ssd1306.display(); // actually display all of the above

  delay(5000);

  Serial.println("Setup completed");
}

int status = -1;

int yOffset = 0;
char dataBuffer[128];

unsigned long lastPing = 0;
const int RADIUS = 16;
int nbSec = 0;

const int MAX_STEPS = 3;

float degToRadians(float deg) {
  return (deg * 71) / 4068;
}

void drawWatch(unsigned long ms, boolean secondsOnly) {
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
  hours %= 12;

  ssd1306.clearDisplay();
  // Face
  for (int deg = 0; deg < 360; deg++) {
    float rad =  degToRadians((float)deg);
    int x = (int)(RADIUS * sin(rad));
    int y = (int)(RADIUS * cos(rad));
    ssd1306.drawPixel(16 + x, 16 + y, WHITE);
  }
  float hoursInDegrees = ((hours % 12) * 30) + (((minutes * 6) + (seconds / 10)) / 12);
  float minInDegrees = ((minutes * 6) + (seconds / 10));
  float secInDegrees = seconds * 6;

//sprintf(dataBuffer, "ms:%d => %02d:%02d:%02d, %f", ms, hours, minutes, seconds, secInDegrees);
//Serial.println(dataBuffer);

  // Seconds
  {
    float rad = degToRadians(secInDegrees);
    int x = round((RADIUS * 0.75) * sin(rad));
    int y = round((RADIUS * 0.75) * cos(rad));
//    if (seconds == 7 || seconds == 8 || seconds == 22 || seconds == 23 || seconds == 37 || seconds == 38 || seconds == 52 || seconds == 53) {
//      sprintf(dataBuffer, "Sec: %d => deg: %f, rad: %f, x: %d, y: %d", seconds, secInDegrees, rad, x, y);
//      Serial.println(dataBuffer);
//    }
    drawLine(16, 16, 16 + x, 16 - y);
  }
  if (!secondsOnly) {
    // Minutes
    {
      float rad = degToRadians(minInDegrees);
      int x = round((RADIUS * 0.65) * sin(rad));
      int y = round((RADIUS * 0.65) * cos(rad));
      drawLine(16, 16, 16 + x, 16 - y);
    }
    // Hours
    {
      float rad = degToRadians(hoursInDegrees);
      int x = round((RADIUS * 0.5) * sin(rad));
      int y = round((RADIUS * 0.5) * cos(rad));
      drawLine(16, 16, 16 + x, 16 - y);
    }
  }
}

int interval = 5000; // default

void loop() {
  unsigned long time = millis();

  if (time - lastPing > interval) {
    status += 1;
    lastPing = time;
    if (status > MAX_STEPS) {
      status = 0;
    }
    Serial.print("Status now ");
    Serial.println(status);
  }

  switch (status) {
    case 0:
      interval = 5000;
      ssd1306.clearDisplay();
      ssd1306.setCursor(0, 0);
      ssd1306.setTextSize(2);
      ssd1306.println("1 - Size 2");
      ssd1306.setTextSize(1);
      ssd1306.println("Oliv did it.");
      sprintf(dataBuffer, "Up: %s", msToTimeDisplay(time - started));
      ssd1306.println(dataBuffer);
      break;
    case 1:
      interval = 2000;
      ssd1306.clearDisplay();
      ssd1306.setCursor(0, yOffset);
      ssd1306.setTextSize(1);
      sprintf(dataBuffer, "2 - yOffset = %d", yOffset);
      yOffset += 1;
      if (yOffset > 32) {
        yOffset = 0;
      }
      ssd1306.println(dataBuffer);
      break;
    case 2:
      interval = 2000;
      ssd1306.clearDisplay();
      ssd1306.setCursor(0, 0);
      ssd1306.setTextSize(3);
      ssd1306.println("Text 3");
      ssd1306.setTextSize(1);
      ssd1306.println("3 - Very big, hey?");
      break;
    case 3: // Bonus
      interval = 60000;
      drawWatch(time - started, false);
      ssd1306.setTextSize(1);
      ssd1306.setCursor(48, 12); // 12: 16 - (8/2)
      sprintf(dataBuffer, "%s", msToTimeStr(time - started));
      ssd1306.println(dataBuffer);
      break;
    default:
      break;
  }

  delay(10);
  yield(); // Send in background and move on
  ssd1306.display();
}

