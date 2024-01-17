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
#include "NMEAParser.h"

#define RX_PIN 0    // White
#define TX_PIN 1    // Green - Not necessary if you do read only.

SoftwareSerial gps = SoftwareSerial(RX_PIN, TX_PIN);

#define VERBOSE false
#define SENTENCE_MAX_LEN 512

#define DETECT_SENTENCE_START 1
#define DETECT_SENTENCE_END   2

#define DETECTION_OPTION DETECT_SENTENCE_START

char sentence[SENTENCE_MAX_LEN];
int sentenceIdx = 0;

#define PARSER_TEST false

void setup() {
  Serial.begin(9600);
  while (!Serial) ;

  if (PARSER_TEST) {
    // Parser tests
    String rmc = "$IIRMC,022136,A,3730.092,N,12228.864,W,00.0,181,141113,15,E,A*1C";
    //           "$IIRMC,144432.086,V,,,,,00.0,0.00,190214,,,N*5F";

    boolean valid = isValid(rmc);
    Serial.print("RMC is "); Serial.println(valid ? "valid" : "not valid");
    if (valid) {
      Rmc rmcStruct;
      initRMC(&rmcStruct);
      parseRMC(rmc, &rmcStruct);
      dumpRMC(rmcStruct, rmc);
    }
  }
  //  pinMode(RX_PIN, INPUT);
  //  pinMode(TX_PIN, OUTPUT);

  gps.begin(9600);
  // gps.listen();

  Serial.println("Setup completed");

  initSentence();
}

void initSentence() {
  for (int i = 0; i < SENTENCE_MAX_LEN; i++) {
    sentence[i] = 0;
  }
  sentenceIdx = 0;
}

void initRMC(Rmc * rmc) {
  rmc->active = false;
  rmc->latitude = 0;
  rmc->longitude = 0;
  rmc->sog = 0.0;
  rmc->cog = 0.0;
  rmc->declination = 0.0;
  rmc->utc.day = 0;
  rmc->utc.month = 0;
  rmc->utc.year = 0;
  rmc->utc.hours = 0;
  rmc->utc.minutes = 0;
  rmc->utc.seconds = 0;
}

const String MONTH[] {
  "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
  "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};

void dumpRMC(Rmc rmc, String nmea) {
  if (rmc.active) {
    Serial.print("\nRMC Sentence:");
    Serial.println(nmea);
    Serial.println("Position:");
    Serial.print("\tLat:"); Serial.println(rmc.latitude);
    Serial.print("\tLng:"); Serial.println(rmc.longitude);
    Serial.println("Velocity:");
    Serial.print("\tcog:"); Serial.println(rmc.cog);
    Serial.print("\tsog:"); Serial.println(rmc.sog);
    Serial.println("Time:");
    Serial.print("\tUTC:"); Serial.print(rmc.utc.day < 10 ? "0" : ""); Serial.print(rmc.utc.day); Serial.print("-"); Serial.print(MONTH[rmc.utc.month -1]); Serial.print("-"); Serial.print(rmc.utc.year); Serial.print(" ");
    Serial.print(rmc.utc.hours < 10 ? "0" : ""); Serial.print(rmc.utc.hours); Serial.print(":"); Serial.print(rmc.utc.minutes < 10 ? "0" : ""); Serial.print(rmc.utc.minutes); Serial.print(":"); Serial.print(rmc.utc.seconds < 10 ? "0" : ""); Serial.print(rmc.utc.seconds); Serial.println();
    Serial.print("D:"); Serial.println(rmc.declination);
  } else {
    Serial.print("\nRMC Not Active: ");
    Serial.println(nmea);
  }
}

int nbPoint = 0;

void loop() {
  //  gps.listen();
  if (gps.available() > 0) {
    char ch = (char)gps.read();
    if (VERBOSE) {
      Serial.print(ch);
      Serial.print(" Ox"); Serial.print(ch < 16 ? "0" : ""); Serial.println(ch, HEX);
    }
    if (DETECTION_OPTION == DETECT_SENTENCE_START) {
      if (ch == '$') { // Start of a sentence
        if (sentenceIdx > 0) { // There is already a sentence
          String nmea = String(sentence);
          nmea.trim();
          if (nmea.length() > 7) {
            String sentenceId = nmea.substring(3, 6);
            if (VERBOSE) {
              Serial.print(sentenceId); Serial.print(" -> "); Serial.println(nmea);
            }
            // Parse!
            if (sentenceId.equals("RMC")) {
              boolean valid = isValid(nmea);
              if (VERBOSE) {
                Serial.print("RMC is "); Serial.println(valid ? "valid" : "not valid");
              }
              if (valid) {
                Rmc rmcStruct;
                initRMC(&rmcStruct);
                parseRMC(nmea, &rmcStruct);
                dumpRMC(rmcStruct, nmea);
                nbPoint = 0;
              }
            } else {
              Serial.print(".");
              nbPoint++;
              if (nbPoint > 80) {
                Serial.println();
                nbPoint = 0;
              }
            }
          }
          initSentence();
        }
      }
    }

    sentence[sentenceIdx++] = ch;

    if (DETECTION_OPTION == DETECT_SENTENCE_END) {
      //  if (sentenceIdx > 1 && sentence[sentenceIdx - 2] == 0x0D && sentence[sentenceIdx - 1] == 0x0A) { // End of Sentence
      if (ch == 0x0A) { // End of Sentence
        String nmea = String(sentence);
        nmea.trim();
        Serial.println(nmea);
        initSentence();
      }
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
