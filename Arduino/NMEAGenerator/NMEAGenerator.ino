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

char sentence[SENTENCE_MAX_LEN];

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

String generateMWT(String deviceID, float temparature) {
  String mwtSentence = "$" + deviceID + "MTW," + String(temparature, 1) + ",C*";
  int cs = calculateCheckSum(mwtSentence);
  mwtSentence = mwtSentence + toHex(cs);
  return mwtSentence;
}

/*
    XDR - Transducer Measurements
      $--XDR,a,x.x,a,c--c,...    ...a,x.x,a,c--c*hh<CR><LF>
             | |   | |    |        ||     |
             | |   | |    |        |+-----+-- Transducer 'n'
             | |   | |    +--------+- Data for variable # of transducers
             | |   | +- Transducer #1 ID (like 0..n, or PTCH, ROLL, ...)
             | |   +- Units of measure, Transducer #1
             | +- Measurement data, Transducer #1
             +- Transducer type, Transducer #1
      Notes:
      1) Sets of the four fields 'Type-Data-Units-ID' are allowed for an undefined number of transducers.
      Up to 'n' transducers may be included within the limits of allowed sentence length, null fields are not
      required except where portions of the 'Type-Data-Units-ID' combination are not available.
      2) Allowed transducer types and their units of measure are:
      Transducer           Type Field  Units Field              Comments
      -------------------------------------------------------------------
      temperature            C           C = degrees Celsius
      angular displacement   A           D = degrees            "-" = anti-clockwise
      linear displacement    D           M = meters             "-" = compression
      frequency              F           H = Hertz
      force                  N           N = Newton             "-" = compression
      pressure               P           B = Bars, P = Pascal   "-" = vacuum
      flow rate              R           l = liters/second
      tachometer             T           R = RPM
      humidity               H           P = Percent
      volume                 V           M = cubic meters
      generic                G           none (null)            x.x = variable data
      current                I           A = Amperes
      voltage                U           V = Volts
      switch or valve        S           none (null)            1 = ON/ CLOSED, 0 = OFF/ OPEN
      salinity               L           S = ppt                ppt = parts per thousand

      Non-numeric transducer IDs seem to be used as needed, like PTCH, PITCH, ROLL, MAGX, MAGY, MAGZ, AIRT, ENGT, ...
      I found no list of "official" transducer IDs.
   */

String generateXDR(String deviceID, float temperature, float salinity) {
  String sensorName = "FIREBEETLE";
  String xdrSentence = "";
// We generate a sentence like $AEXDR,C,12.3,C,FIREBEETLE,L,23.45,S,FIREBEETLE*65
  xdrSentence = "$" + deviceID + "XDR,C," + String(temperature, 1) +
                  ",C," + sensorName +
                  ",L," + String(salinity, 2) + ",S," + sensorName + "*";
  int cs = calculateCheckSum(xdrSentence);
  xdrSentence = xdrSentence + toHex(cs);
  return xdrSentence;
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

      if (false) {
        Serial.print("Temperature is ");
        Serial.print(temp, 2);
        Serial.print(char(176));
        Serial.print("C");
        Serial.println("");
        Serial.println(temp, 6);
      }
      String deviceID = "AE"; // Astrolabe Expeditions
      String mwtSentence = generateMWT(deviceID, temp);
      // With T=12.345, expect $AEMTW,12.3,C*17
      Serial.println("Generated MWT Sentence: " + mwtSentence);

      String xdrSentence = generateXDR(deviceID, temp, 23.45); // Salinity hard-coded
      // Expect $AEXDR,C,12.3,C,FIREBEETLE,L,23.45,S,FIREBEETLE*65
      Serial.println("Generated XDR Sentence: " + xdrSentence);

    }
    receivedSentence = ""; // Reset
    // delay ?
	delay(1000);
}