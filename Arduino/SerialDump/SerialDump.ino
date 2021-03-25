#include <SoftwareSerial.h>  // See https://www.arduino.cc/en/Reference/softwareSerial

/*
   GPS Wiring, Serial port (USB, refer to the coors below).

   White on pin 0 (RX <- 0) Receives from device (like GPS)
   Green on pin 1 (TX -> 1)
   Red on 5V
   Black on GND

   Note: if you see a
     avrdude: stk500_getsync() attempt 1 of 10: not in sync: resp=0x00
   when uploading the sketch, unplug the 5V from the GPS, and try again.

*/

#define RX_PIN 0    // White
#define TX_PIN 1    // Green - Not necessary if you do read only.

SoftwareSerial gps = SoftwareSerial(RX_PIN, TX_PIN);

#define VERBOSE false
#define SENTENCE_MAX_LEN 512

char sentence[SENTENCE_MAX_LEN];
int sentenceIdx = 0;

void setup() {
  //  pinMode(RX_PIN, INPUT);
  //  pinMode(TX_PIN, OUTPUT);

  gps.begin(9600);
  // gps.listen();

  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  Serial.println("Setup completed");

  initSentence();
}

void initSentence() {
  for (int i = 0; i < SENTENCE_MAX_LEN; i++) {
    sentence[i] = 0;
  }
  sentenceIdx = 0;
}

void loop() {
  //  gps.listen();
  if (gps.available() > 0) {
    char ch = (char)gps.read();
    if (VERBOSE) {
      Serial.print(ch);
      Serial.print(" Ox"); Serial.print(ch < 16 ? "0" : ""); Serial.println(ch, HEX);
    }
    
    sentence[sentenceIdx++] = ch;
//  if (sentenceIdx > 1 && sentence[sentenceIdx - 2] == 0x0D && sentence[sentenceIdx - 1] == 0x0A) { // End of Sentence
    if (ch == 0x0A) { // End of Sentence
      String nmea = String(sentence);
      nmea.trim();
      Serial.println(nmea);
      initSentence();
    }

  } else {
    delay(1000);
    if (VERBOSE) {
      Serial.println("No GPS data");
      if (gps.isListening()) {
        Serial.println("\tGPS IS listening");
      } else {
        Serial.println("\tGPS is NOT listening");
      }
    }
  }
}
