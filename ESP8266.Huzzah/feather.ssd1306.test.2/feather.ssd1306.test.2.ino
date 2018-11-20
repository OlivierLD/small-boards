/**
   Screen test for the Feather SSD1306 128x32, or 128x64
   Screen definition and address depend on defines: SSD1306_128x32 and SSD1306_128x64, see below.
   Graphical primitives doc at https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives
   
   Test with NO buttons

   @author Olivier LeDiouris
*/
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// for 128x32
//#define SSD1306_128x32
//#undef SSD1306_128x64

// for 128x64
#undef SSD1306_128x32
#define SSD1306_128x64

const int SSD1306_WIDTH = 128;

#ifdef SSD1306_128x32
const int SSD1306_I2C_ADDR = 0x3C; // for 128x32
const int SSD1306_HEIGHT = 32;
#endif

#ifdef SSD1306_128x64
const int SSD1306_I2C_ADDR = 0x3D; // for 128x64
const int SSD1306_HEIGHT = 64;
#endif

Adafruit_SSD1306 ssd1306 = Adafruit_SSD1306(SSD1306_WIDTH, SSD1306_HEIGHT, &Wire);

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

void setup() {
  started = millis(); // init.
  ssd1306.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDR);
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
const int RADIUS = SSD1306_HEIGHT / 2;
const int CENTER_X = SSD1306_HEIGHT / 2;
const int CENTER_Y = SSD1306_HEIGHT / 2;
int nbSec = 0;

const int MAX_STEPS = 4;

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
    ssd1306.drawPixel(CENTER_X + x, CENTER_Y + y, WHITE);
  }
  // Other option, faster
  ssd1306.drawCircle(CENTER_X, CENTER_Y, (int)(RADIUS * 0.9), WHITE);
  
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
    ssd1306.drawLine(CENTER_X, CENTER_Y, CENTER_X + x, CENTER_Y - y, WHITE);
  }
  if (!secondsOnly) {
    // Minutes
    {
      float rad = degToRadians(minInDegrees);
      int x = round((RADIUS * 0.65) * sin(rad));
      int y = round((RADIUS * 0.65) * cos(rad));
      ssd1306.drawLine(CENTER_X, CENTER_Y, CENTER_X + x, CENTER_Y - y, WHITE);
    }
    // Hours
    {
      float rad = degToRadians(hoursInDegrees);
      int x = round((RADIUS * 0.5) * sin(rad));
      int y = round((RADIUS * 0.5) * cos(rad));
      ssd1306.drawLine(CENTER_X, CENTER_Y, CENTER_X + x, CENTER_Y - y, WHITE);
    }
  }
}

int interval = 5000; // default

int bouncingBallX = random(10, SSD1306_WIDTH - 10);
int bouncingBallY = 0;
int bouncingXIncrement = 1;
int bouncingYIncrement = 3;

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
      interval = 2000 * (SSD1306_HEIGHT / 32);
      ssd1306.clearDisplay();
      ssd1306.setCursor(0, yOffset);
      ssd1306.setTextSize(1);
      sprintf(dataBuffer, "2 - yOffset = %d", yOffset);
      yOffset += 1;
      if (yOffset > SSD1306_HEIGHT) {
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
      {
        drawWatch(time - started, false);
        ssd1306.setTextSize(1);
        int xCursor = (2 * RADIUS) + 12; // 48 in 128x32
        int yCursor = RADIUS - 4; // 4=8/2. 12 in 128x32
        ssd1306.setCursor(xCursor, yCursor - 10);
        ssd1306.println("Up since");
        ssd1306.setCursor(xCursor, yCursor);
        sprintf(dataBuffer, "%s", msToTimeStr(time - started));
        ssd1306.println(dataBuffer);
      }
      break;
    case 4: // Bouncing ball
      interval = 30000;
      ssd1306.clearDisplay();
      // Frame
//    ssd1306.drawFastHLine(0, 0, SSD1306_WIDTH - 1, WHITE); // Top
//    ssd1306.drawFastHLine(0, SSD1306_HEIGHT - 1, SSD1306_WIDTH, WHITE); // Bottom
//    ssd1306.drawFastVLine(0, 0, SSD1306_HEIGHT - 1, WHITE); // Left
//    ssd1306.drawFastVLine(0, SSD1306_WIDTH - 1, SSD1306_HEIGHT, WHITE); // Right
      ssd1306.drawRect(0, 0, SSD1306_WIDTH, SSD1306_HEIGHT, WHITE);
      
      ssd1306.fillCircle(bouncingBallX, bouncingBallY, 3, WHITE);
      bouncingBallX += bouncingXIncrement;
      bouncingBallY += bouncingYIncrement;
      if (bouncingBallX <= 0 || bouncingBallX >= SSD1306_WIDTH) {
        bouncingXIncrement *= -1;
      }
      if (bouncingBallY <= 0 || bouncingBallY >= SSD1306_HEIGHT) {
        bouncingYIncrement *= -1;
      }
      break;  
    default:
      break;
  }

  delay(10);
  yield(); // Send in background and move on
  ssd1306.display();
}

