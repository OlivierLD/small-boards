/**
 * Types used by the NMEA Parser
 */
#ifndef NMEA_DATA_TYPES

#define NMEA_DATA_TYPES

typedef struct DATE_TIME {
  int year;
  int month;
  int day;
  int hours;
  int minutes;
  float seconds;
} DateTime;

typedef struct RMC {
  bool active = false;
  double longitude;
  double latitude;
  double sog;
  double cog;
  // Date & Time
  DateTime utc;
  double declination;
} Rmc;

#endif

int calculateCheckSum(String sentence);
String toHex(int val);
bool isValid(String sentence);
String generateMTW(String talkerID, float temperature);
String generateXDR(String talkerID, float temperature, float salinity);