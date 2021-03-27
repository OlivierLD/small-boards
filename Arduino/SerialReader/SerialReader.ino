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
  while (!Serial) ;
  Serial.println("Setup completed");
  Serial.println("---------------");
}

String nmeaSentence = "";
String END_OF_NMEA = "\r\n";
String START_OF_NMEA = "$";

int nbPoint = 0;

void loop() {
  //  gps.listen();
  if (gps.available() > 0) {
    char ch = (char)gps.read();

    //    if (ch == '$') { // It happens when the sentence was not finished with \r\n.
    //      if (nmeaSentence.length() > 0) {
    //        nmeaSentence.trim();
    //        Serial.print(">> Early end: ");
    //        Serial.println(nmeaSentence);
    //        nmeaSentence = "";
    //      }
    //    }

    nmeaSentence.concat(ch);

    if (nmeaSentence.endsWith(END_OF_NMEA)) {
      nmeaSentence.trim();
      if (nmeaSentence.substring(3).startsWith("RMC")) {
        //Serial.println("---------------------------------");
        Serial.println();
        Serial.println(nmeaSentence);
        //Serial.println("---------------------------------");
        nbPoint = 0;
      } else {
        //Serial.println(nmeaSentence);
        Serial.print(".");
        nbPoint++;
        if (nbPoint > 80) {
          Serial.println();
          nbPoint = 0;
        }
      }
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
