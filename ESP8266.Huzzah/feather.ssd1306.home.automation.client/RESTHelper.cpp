#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>

#include "RESTHelper.h"

#ifndef NULL
#define NULL 0
#endif

#ifndef DEBUG
#define DEBUG false
#endif

// Private vars, in the header (RESTHelper.h)
// boolean simulating = false;
// char dataBuffer[256];

RESTHelper::RESTHelper(boolean simulate) {
  simulating = simulate;
}

String RESTHelper::makeRESTRequest(String host, int port, String url, String verb, String payload) {
  String response;

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = port;
  if (!simulating && !client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return "{\"error\":\"Connection failed\"}";
  }

  if (payload != NULL) {
    sprintf(dataBuffer, "Content.length: %d", payload.length());
    String headers[] = {
      "Content-Type: application/json",
      dataBuffer
    };
    sendRESTRequest(client, verb, url, "HTTP/1.1", host, headers, 2, payload);
  } else {
    sendRESTRequest(client, verb, url, "HTTP/1.1", host, NULL, 0, payload);
  }
  delay(500);

  int status = 0;
  if (!simulating) {
    // Read all the lines of the reply from server and print them to Serial
    while (client.available()) {
      response = client.readStringUntil('\r');
      Serial.print(response); // Output here
      if (response.startsWith("HTTP/1.1 ")) {
        status = response.substring(9).toInt();
      }
    }
    // Last line (payload) is returned, only. Headers a read, but not returned.
    Serial.println();
    Serial.println("-- REST request response received. --");
    Serial.print("  >>> Response Status:");
    Serial.println(status);
  } else {
    Serial.println("... Simulated response");
    response = "{simulated: true}";
  }
  Serial.println();
  Serial.println("closing connection");

  return response;
}

void RESTHelper::sendRESTRequest(WiFiClient client, String verb, String url, String protocol, String host, String headers[], int headerLen, String payload) {
  String request = verb + " " + url + " " + protocol + "\r\n" +
                   "Host: " + host + "\r\n";
  // Headers ?
  if (headers != NULL && headerLen > 0) {
    for (int i = 0; i < headerLen; i++) {
      request = String(request + headers[i] + "\r\n");
    }
  }

  request = String(request + "Connection: close\r\n" + "\r\n");

  // Payload ?
  if (payload != NULL && payload.length() > 0) {
    request = String(request + payload);
  } else if (DEBUG) {
    Serial.println("No Payload");
  }
  if (DEBUG) {
    Serial.println("--------------------");
    Serial.println(request);
    Serial.println("--------------------");
  }
  if (!simulating) {
    client.print(request);
  }
}

