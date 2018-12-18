#ifndef REST_Helper
#define REST_Helper

#include <Arduino.h>

class RESTHelper {
  public:
    typedef struct RESPONSE {
      int status;
      String content;
    } Response;
  public:
    RESTHelper(boolean simulate);
    Response makeRESTRequest(String host, int port, String url, String verb, String payload);
    void sendRESTRequest(WiFiClient client, String verb, String url, String protocol, String host, String headers[], int headerLen, String payload);
  private:
    boolean simulating = false;
    char dataBuffer[256];
};

#endif
