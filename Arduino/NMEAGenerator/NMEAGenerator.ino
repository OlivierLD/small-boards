/*
   NMEA Sentence generation. WiP.
   MTW & XDR

   Requires interaction with the Serial Console.
   Enter a temperature, and see the generated NMEA sentences.
*/
#include "NMEAParser.h"

#define VERBOSE false
#define SENTENCE_MAX_LEN 512

char sentence[SENTENCE_MAX_LEN];

String EOS = "\r\n";
String receivedSentence = "";

void setup() {
    Serial.begin(9600);
    while (!Serial) ;
    Serial.print("Setup completed !\n");
    Serial.print("Enter temperature (Serial input).\n");
}

void loop() {
    int data = -1;
    while (Serial.available() > 0) { // Checks whether data is coming from the serial port
        data = Serial.read(); // Reads the data from the serial port (can be a bluetooth port)
        receivedSentence.concat((char)data);
    }
    // Received a String
    if (receivedSentence.length() > 0) {
      if (receivedSentence.endsWith(EOS)) {
          receivedSentence = receivedSentence.substring(0, receivedSentence.length() - EOS.length());
      }
      receivedSentence.toUpperCase();
      // TODO Manage it
      Serial.print("Received: " + receivedSentence + "\n");
   	  // Serial.print("\n");
      float temp = receivedSentence.toFloat();

      if (VERBOSE) {
        Serial.print("Temperature is ");
        Serial.print(temp, 2);
        Serial.print(char(176));
        Serial.print("C");
        Serial.println("");
        Serial.println(temp, 6);
      }
      String talkerID = "AE"; // Astrolabe Expeditions
      String mwtSentence = generateMTW(talkerID, temp);
      // With T=12.345, expect $AEMTW,12.3,C*17
      Serial.println("Generated MWT Sentence: " + mwtSentence);

      String xdrSentence = generateXDR(talkerID, temp, 23.45); // Salinity hard-coded
      // Expect $AEXDR,C,12.3,C,FIREBEETLE,L,23.45,S,FIREBEETLE*65
      Serial.println("Generated XDR Sentence: " + xdrSentence);

    }
    receivedSentence = ""; // Reset
    // delay ?
	delay(1000);
}