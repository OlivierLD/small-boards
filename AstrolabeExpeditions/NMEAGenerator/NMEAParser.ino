/**
 * NMEA Utilities...
 */

/**
 * @brief Calculate the checksum of. given sentence
 *
 * @param sentence
 * @return int IN DECIMAL !!!
 */
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

/**
 * @brief Decimal to Hexadecimal conversion.
 *
 * @param val int, decimal.
 * @return String, 2 characters.
 */
String toHex(int val) {
  String hexVal = String(val, 16);
  if (val < 16) {
    hexVal = "0" + hexVal;
  }
  return hexVal;
}

/**
 * @brief Validates a given NMEA sentence by checking its checksum.
 *
 * @param sentence
 * @return bool true when valid, false otherwise
 */
bool isValid(String sentence) {
  //  Serial.print("NMEA:"); Serial.println(sentence);
  bool valid = true;
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

/**
 * @brief Generates an MTW NMEA Sentence
  MTW - Water Temperature
    $--MTW,x.x,C*hh<CR><LF>
     | |   |   |
     | |   |   +-- Unit of measurement, C = degrees Celsius
     | |   +----- Temperature, degrees Celsius
     | +------- MWV NMEA sentence type
     +--------- NMEA Talker ID
  Example: $AEMTW,12.3,C*17
 *
 * @param talkerID
 * @param temperature
 * @return String
 */
String generateMTW(String talkerID, float temperature) {
  String mwtSentence = "$" + talkerID + "MTW," + String(temperature, 1) + ",C*";
  int cs = calculateCheckSum(mwtSentence);
  String checksum = toHex(cs);
  checksum.toUpperCase();
  mwtSentence = mwtSentence + checksum;
  return mwtSentence;
}

/**
 * @brief Generates an XDR NMEA Sentence
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
 *
 * @param talkerID
 * @param temperature in Celsius
 * @param salinity in ppt (parts per thousand)
 * @return String
 */
String generateXDR(String talkerID, float temperature, float salinity) {
  // String sensorName = "FIREBEETLE"; // Hard-coded sensor name, for now.
  String tsgSensorName = "TSG"; // thermosalinographe
	String ctdSensorName = "CTD"; // Conductivity Temperature Deph

  String xdrSentence = "";
  // We generate a sentence like $AEXDR,C,12.3,C,FIREBEETLE,L,23.45,S,FIREBEETLE*65
  //                          or $AEXDR,C,12.3,C,TSG,L,23.45,S,CTD*76
  xdrSentence = "$" + talkerID + "XDR,C," + String(temperature, 1) +
                ",C," + ctdSensorName +
                ",L," + String(salinity, 2) + ",S," + tsgSensorName + "*";
  int cs = calculateCheckSum(xdrSentence);
  String checksum = toHex(cs);
  checksum.toUpperCase();
  xdrSentence = xdrSentence + checksum;
  return xdrSentence;
}

/**
 * @brief Generates an MDA NMEA Sentence
 *
 * MDA Meteorological Composite
 * $--MDA,x.x,I,x.x,B,x.x,C,x.x,C,x.x,x.x,x.x,C,x.x,T,x.x,M,x.x,N,x.x,M*hh<CR><LF>
 *        |     |     |     |     |   |   |     |     |     |     |
 *        |     |     |     |     |   |   |     |     |     |     19-Wind speed, m/s
 *        |     |     |     |     |   |   |     |     |     17-Wind speed, knots
 *        |     |     |     |     |   |   |     |     15-Wind dir Mag
 *        |     |     |     |     |   |   |     13-Wind dir, True
 *        |     |     |     |     |   |   11-Dew Point C
 *        |     |     |     |     |   10-Absolute hum %
 *        |     |     |     |     9-Relative hum %
 *        |     |     |     7-Water temp in Celsius
 *        |     |     5-Air Temp in Celsius  |
 *        |     3-Pressure in Bars
 *        1-Pressure in inches
 *
 * Example: $WIMDA,29.4473,I,0.9972,B,17.2,C,,,,,,,,,,,,,,*3E
 *
 * Note: We generate here only pressure and air temperature values. More can be added if needed.
 *
 * @param talkerID
 * @param pressure in Bars
 * @param temperature in Celsius
 * @return String
 */

String generateMDA(String talkerID, float pressure, float temperature) {
  String mdaSentence = "$" + talkerID + "MDA," +
                       String(pressure / 33.8639, 4) + ",I," + // 1-Pressure in inches
                       String(pressure, 4) + ",B," +            // 3-Pressure in Bars
                       String(temperature, 1) + ",C," +         // 5-Air Temp in Celsius
                       ",,,,,,,,,,,,,,,*";                      // The rest is empty for now
  int cs = calculateCheckSum(mdaSentence);
  String checksum = toHex(cs);
  checksum.toUpperCase();
  mdaSentence = mdaSentence + checksum;
  return mdaSentence;
}

/**
 * @brief Generates an MMB NMEA Sentence
 * MMB - Atmospheric/Barometric Pressure
 *   $--MMB,x.x,I,x.x,B*hh<CR><LF>
 *   |  |   |   | |   |
 *   |  |   |   | |   +-- Unit of measurement, B = Bars
 *   |  |   |   | +----- Pressure, Bars
 *   |  |   |   +--------- Unit of measurement, I = inches of Hg
 *   |  |   +------------- Pressure, inches of Hg
 *   |  +----------------- MMB NMEA sentence type
 *   +------------------- NMEA Talker ID
 * Example: $IIMMB,29.9350,I,1.0136,B*7A
 * Note: 1 inch of Hg = 33.8639 millibar (hPa)
 * Thus, to convert millibar to inches of Hg, divide by 33.8639
 * To convert inches of Hg to millibar, multiply by 33.8639
 * Example: 1013.6 millibar = 29.9350 inches of Hg
 * 29.9350 inches of Hg = 1013.6 millibar
 * Reference: https://www.weather.gov/media/epz/wxcalc/pressureConversion.pdf
 *
 * @param talkerID
 * @param pressure in Bars !!
 * @return String
 */
String generateMMB(String talkerID, float pressure) {
  String mmbSentence = "$" + talkerID + "MMB," + String(pressure / 33.8639, 4) + ",I," + String(pressure, 4) + ",B*";
  int cs = calculateCheckSum(mmbSentence);
  String checksum = toHex(cs);
  checksum.toUpperCase();
  mmbSentence = mmbSentence + checksum;
  return mmbSentence;
}

/**
 * @brief generates an MTA NMEA Sentence
  MTA - Air Temperature
    $--MTA,x.x,C*hh<CR><LF>
     | |   |   |
     | |   |   +-- Unit of measurement, C = degrees Celsius
     | |   +----- Temperature, degrees Celsius
     | +------- MTA NMEA sentence type
     +--------- NMEA Talker ID
  Example: $AEMTA,15.2,C*1C
 *
 * @param talkerID
 * @param temperature in Celsius
 * @return String
 */
String generateMTA(String talkerID, float temperature) {
  String mtaSentence = "$" + talkerID + "MTA," + String(temperature, 1) + ",C*";
  int cs = calculateCheckSum(mtaSentence);
  String checksum = toHex(cs);
  checksum.toUpperCase();
  mtaSentence = mtaSentence + checksum;
  return mtaSentence;
}