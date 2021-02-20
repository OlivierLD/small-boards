#include <SoftwareSerial.h>  // See https://www.arduino.cc/en/Reference/softwareSerial

/*
   GPS Wiring

   White on pin 0 (RX <- 0) Receives from device (like GPS)
   Green on pin 1 (TX -> 1)
   Red on 5V
   Black on GND

*/

#define RX_PIN 0    // White
#define TX_PIN 1    // Green - Not necessary if you do read only.

SoftwareSerial gps = SoftwareSerial(RX_PIN, TX_PIN);

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
}

String nmeaSentence = "";
String END_OF_NMEA = "\r\n";

void loop() {
  //  gps.listen();
  if (gps.available() > 0) {
    char ch = (char)gps.read();
    nmeaSentence.concat(ch);

    if (nmeaSentence.endsWith(END_OF_NMEA)) {
      nmeaSentence.trim();
      Serial.println(nmeaSentence);
      nmeaSentence = "";
    }
    // Serial.println(ch);
  } else {
    delay(1000);
    if (!gps.isListening()) {
      Serial.println("No GPS data");
      Serial.println("\tGPS is NOT listening");
    }
  }
}
