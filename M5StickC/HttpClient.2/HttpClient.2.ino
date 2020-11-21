#include <SPI.h>
#include <WiFi.h>
#include <M5StickC.h>

/*
   Make HTTP requests to wifitest.adafruit.com.
   ------------------
   RST Button: top right
*/

// change values below to fit your settings
const char* SSID = "Sonic-00e0_EXT";        // your network SSID (name)
const char* PASSWORD = "67369c7831";        // your network password
const char * SERVER_NAME = "wifitest.adafruit.com";  // For REST requests
const int SERVER_PORT = 80;


int status = WL_IDLE_STATUS;

WiFiClient client;

void setup() {
  M5.begin();
  Serial.begin(9600);

  status = WiFi.begin(SSID, PASSWORD);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.printf(".");
    Serial.print(".");
  }

  M5.Lcd.printf("Connected to wifi");
  Serial.println("\nConnected to wifi");

  pinMode(M5_BUTTON_RST, INPUT);
}

void makeRequest(String verb, String request) {
  // if you get a connection, report back via serial:
  Serial.println("Connecting...");
  if (client.connect(SERVER_NAME, SERVER_PORT)) {
    Serial.println("Connected!");
    // Make a HTTP/REST reques, cannot be more explicit! :
    String restRequest =
              verb + " " + request + " HTTP/1.1\r\n" +
              "Host: " + SERVER_NAME + ":" + String(SERVER_PORT) + "\r\n" +
              "Connection: close\r\n" +
              "\r\n"; //  + body + "\r\n";
    Serial.println(restRequest);
    client.print(restRequest);
  } else {
    Serial.println("Not connected...");
  }

  delay(500);

  while (client.available()) { // Response?
    Serial.println("\tReading response");
    String line = client.readStringUntil('\n');
    Serial.println(line); // debug
  }
  Serial.println("\tDone reading response");
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println("\tStopping client");
    client.stop();
  }
  Serial.println("\tDone making request");
}

void loop() {

  String display ;
  // change active lamp
  if (digitalRead(M5_BUTTON_RST) == LOW) {
    Serial.println("RST button LOW");

    display = "Making request...";

    M5.Lcd.setRotation( 3 );
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 10);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(3);
    M5.Lcd.print(display);

    makeRequest("GET", "/testwifi/index.html");

    delay(100);
  }

  delay(200);
}
