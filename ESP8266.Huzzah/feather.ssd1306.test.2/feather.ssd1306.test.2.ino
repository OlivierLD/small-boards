/**
   Test for the Feather SSD1306 128x32
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
 
void loop() {
  unsigned long time = millis();

  if (time - lastPing > 5000) { 
    status += 1;
    lastPing = time;
    if (status > 2) {
      status = 0;
    }
    Serial.print("Status now ");
    Serial.println(status);
  }
  
  switch (status) {
    case 0:
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
  yield(); // TODO What's that?
  ssd1306.display();
}

