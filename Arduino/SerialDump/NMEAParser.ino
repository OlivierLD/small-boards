/**

*/

//typedef struct RMC {
//  double longitude;
//  double latitude;
//  double sog;
//  double cog;
//  // Date & Time
//  double declination;
//} Rmc;

// typedef RMC Rmc;

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

int countSep(String str, char separator) {
  int nbSep = 0;
  for (int i = 0; i < str.length(); i++) {
    if (str.charAt(i) == separator) {
      nbSep += 1;
    }
  }
  return nbSep;
}

void split(String original, char separator, String array[]) {
  String tempString;
  int currentSepIndex = -1;
  int currentArrayIndex = 0;
  while (true) {
    int nextIndex = original.indexOf(String(separator), currentSepIndex + 1);
    if (nextIndex != -1) {
      String oneElem = original.substring(currentSepIndex + 1, nextIndex);
      array[currentArrayIndex] = oneElem;
      currentArrayIndex++;
      currentSepIndex = nextIndex;
    } else {
      String oneElem = original.substring(currentSepIndex + 1);
//      Serial.print("Last part ("); Serial.print(currentArrayIndex); Serial.print(") "); Serial.println(oneElem);
      array[currentArrayIndex] = oneElem;
      break;
    }
  }
}

void parseRMC(String sentence, Rmc * rmc) {
  sentence.trim();
  if (isValid(sentence)) {
    String toParse = sentence.substring(1, sentence.indexOf("*"));
    int nbSep = countSep(toParse, ',');
//    Serial.print("Found "); Serial.print(nbSep); Serial.println(" ','");
    String dataArray[nbSep + 1];
    split(toParse, ',', dataArray);
//    for (int i = 0; i < nbSep + 1; i++) {
//      Serial.print(i); Serial.print(" -> ["); Serial.print(dataArray[i]); Serial.println("]");
//    }
    //
    if (!dataArray[2].equals("A")) { // Active/Void
      rmc->active = false;
      // Serial.println("Not Active...");
      return;
    }
    rmc->active = true;
    String lat = dataArray[3];
    String latSign = dataArray[4];
    if (lat.length() > 0) {
      int deg = lat.substring(0, 2).toInt();
      double min = lat.substring(2).toDouble();
      min /= 60.0;
//      Serial.print("From:["); Serial.print(lat); Serial.print("] ");
//      Serial.print("Deg:"); Serial.print(deg); Serial.print(" Min:"); Serial.print(min); Serial.println();
      rmc->latitude = (deg + min) * (latSign.equals("S") ? -1 : 1);
    }

    String lng = dataArray[5];
    String lngSign = dataArray[6];
    if (lng.length() > 0) {
      int deg = lng.substring(0, 3).toInt();
      double min = lng.substring(3).toDouble();
      min /= 60.0;
//      Serial.print("From:["); Serial.print(lng); Serial.print("] ");
//      Serial.print("Deg:"); Serial.print(deg); Serial.print(" Min:"); Serial.print(min); Serial.println();
      rmc->longitude = (deg + min) * (lngSign.equals("W") ? -1 : 1);
    }

    String utcTime = dataArray[1]; // Time.
    String utcDate = dataArray[9]; // Date.
    if (utcTime.length() >= 6) {
      rmc->utc.hours = utcTime.substring(0, 2).toInt();
      rmc->utc.minutes = utcTime.substring(2, 4).toInt();
      rmc->utc.seconds = utcTime.substring(4).toFloat();
    }
    if (utcDate.length() == 6) {
      rmc->utc.day = utcDate.substring(0, 2).toInt();
      rmc->utc.month = utcDate.substring(2, 4).toInt();
      rmc->utc.year = utcDate.substring(4).toInt();
      if (rmc->utc.year > 50) {
        rmc->utc.year += 1900;
      } else {
        rmc->utc.year += 2000;
      }
    }

    String sog = dataArray[7];
    String cog = dataArray[8];
    if (sog.length() > 0) {
      rmc->sog = sog.toDouble();
    }
    if (cog.length() > 0) {
      rmc->cog = cog.toDouble();
    }

    if (dataArray[10].length() > 0 && dataArray[11].length() > 0) {
      rmc->declination = dataArray[10].toDouble();
      if (dataArray[11].equals("W")) {
        rmc->declination *= -1;
      }
    }
  } else {
    // Invalid String
    Serial.println("Invalid Sentence");
  }
}
