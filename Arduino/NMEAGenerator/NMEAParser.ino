/*
   NMEA Sentence generation. WiP.

	MTW Water Temperature
	Structure is

	$xxMTW,+18.0,C*hh
	       |     |
	       |     Celsius
	       Temperature

*/
// #include "NMEAParser.h"

#define VERBOSE false
#define SENTENCE_MAX_LEN 512

#define DETECT_SENTENCE_START 1
#define DETECT_SENTENCE_END   2

#define DETECTION_OPTION DETECT_SENTENCE_START

char sentence[SENTENCE_MAX_LEN];
int sentenceIdx = 0;

String EOS = "\r\n";
String receivedSentence = "";

int calculateCheckSum(String sentence) {
  int cs = 0;
  String str2validate = sentence.substring(1, sentence.indexOf("*"));
//  Serial.print("To Validate:"); Serial.println(str2validate);
  int len = str2validate.length();
  for (int i = 0; i < len; i++) {
    char c = str2validate.charAt(i);
    cs = cs ^ c;
  }
  return cs;
}

String toHex(int val) {
  String hexVal = String(val, 16);
  if (val < 16) {
    hexVal = "0" + hexVal;
  }
  return hexVal;
}

boolean isValid(String sentence) {

//  Serial.print("NMEA:"); Serial.println(sentence);

  boolean valid = true;
  sentence.trim();
  if (sentence.startsWith("$")) {
    if (sentence.indexOf("*") > -1) {
      String inLineCheckSum = sentence.substring(sentence.indexOf("*") + 1);
      int cs = calculateCheckSum(sentence);
      String csHex = toHex(cs);
      csHex.toUpperCase();
//      Serial.print("Found:"); Serial.print(inLineCheckSum);
//      Serial.print(" Calculated:"); Serial.print(csHex);
//      Serial.println();
      valid = csHex.equals(inLineCheckSum);
    } else {
      valid = false;
    }
  } else {
    valid = false;
  }
  return valid;
}

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
      Serial.print("Temperature is ");
      Serial.print(temp, 2);
      Serial.print(char(176));
      Serial.print("C");
      Serial.println("");
      Serial.println(temp, 6);

      String deviceID = "AE"; // Astrolabe Expeditions
      String nmeaSentence = "$" + deviceID + "MTW," + String(temp, 1) + ",C*";
      int cs = calculateCheckSum(nmeaSentence);
      nmeaSentence = nmeaSentence + toHex(cs);
      // Expect $AEMTW,12.3,C*17
      Serial.println("Generated NMEA Sentence: " + nmeaSentence);

    }
    receivedSentence = ""; // Reset
    // delay ?
	delay(1000);
}