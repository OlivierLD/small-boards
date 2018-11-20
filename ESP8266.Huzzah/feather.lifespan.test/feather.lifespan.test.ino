/*
   Test the life span of a Feather with OLED Screen, and a LiPo battery.
   POST a REST request every second, as long as it is alive.
   The server keeps track of the messages posted by the Feather.
*/

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define NULL 0
#define DEBUG true
#define SIMULATING false

Adafruit_SSD1306 ssd1306 = Adafruit_SSD1306(128, 32, &Wire);

// Network and Host definitions

// Make sure the 2 lines below match YOUR network
const char* ssid     = "Sonic-00e0";
const char* password = "67369c7831";

// hostname and port where the REST Server is running
const char* host = "192.168.42.8"; // Or whatever IP or hostname you can reach
const int port = 9999;

unsigned long started = 0;

char* msToTimeStr(long ms) {
  int seconds = (int)(ms / 1000);
  int minutes = 0;
  int hours = 0;
  if (seconds > 59) {
    minutes = (int)(seconds / 60);
    seconds -= (minutes * 60);
  }
  if (minutes > 59) {
    hours = (int)(minutes / 60);
    minutes -= (hours * 60);
  }
  char buffer[128];
  sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
  return buffer;
}

void setup() {
  started = millis(); // init.
  ssd1306.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  // initialize display
  ssd1306.display();
  delay(1000);
  ssd1306.clearDisplay();
  ssd1306.display();

  Serial.begin(115200); // Console output
  delay(100);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  if (!SIMULATING) {
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Netmask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("Gateway: ");
    Serial.println(WiFi.gatewayIP());
  } else {
    Serial.println("Simulating Network Connection...");
  }
}

char dataBuffer[128];
unsigned long lastPing = 0;
int interval = 1000; // default

void loop() {
  delay(500);

  unsigned long time = millis();

  ssd1306.clearDisplay();
  ssd1306.setTextColor(WHITE);
  ssd1306.setTextSize(2);
  ssd1306.setCursor(0, 0);
  sprintf(dataBuffer, "%s", msToTimeStr(time - started));
  ssd1306.println(dataBuffer);
  ssd1306.display();

  String payload = dataBuffer; // Used for REST request below

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = port;
  if (!SIMULATING && !client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
//  {
//    String url = "/testwifi/index.html";
//    Serial.print("Requesting URL: ");
//    Serial.println(url);
//
//    // This will send the request to the server
//    sendRequest(client, "GET", url, "HTTP/1.1", host);
//    delay(500);
//    sendRESTRequest(client, "GET", url, "HTTP/1.1", host, NULL, 0, String());
//    delay(500);
//  }

  String url = "/feather/lifespan"; // REST Endpoint
  sprintf(dataBuffer, "Content.length: %d", payload.length());
  String headers[] = {
    "Content-Type: text/plain",
    dataBuffer
  };
  sendRESTRequest(client, "POST", url, "HTTP/1.1", host, headers, 2, payload);
  delay(500);

  if (!SIMULATING) {
    // Read all the lines of the reply from server and print them to Serial
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line); // Output here
    }
  } else {
    Serial.println("... Simulated response");
  }

  Serial.println();
  Serial.println("closing connection");
}

//void sendRequest(WiFiClient client, String verb, String url, String protocol, String host) {
//  String request = verb + " " + url + " " + protocol + "\r\n" +
//                   "Host: " + host + "\r\n" +
//                   "Connection: close\r\n" +
//                   "\r\n";
//  if (DEBUG) {
//    Serial.println("--------------------");
//    Serial.println(request);
//    Serial.println("--------------------");
//  }
//  if (!SIMULATING) {
//    client.print(request);
//  }
//}
//
void sendRESTRequest(WiFiClient client, String verb, String url, String protocol, String host, String headers[], int headerLen, String payload) {
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
  if (!SIMULATING) {
    client.print(request);
  }
}

