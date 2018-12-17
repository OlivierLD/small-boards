#ifndef REST_Helper
#define REST_Helper

#include <Arduino.h>

class RESTHelper {
  public:
    RESTHelper(boolean simulate);
    String makeRESTRequest(String host, int port, String url, String verb, String payload);
    void sendRESTRequest(WiFiClient client, String verb, String url, String protocol, String host, String headers[], int headerLen, String payload);
  private:
    boolean simulating = false;
    char dataBuffer[256];
};

#endif
