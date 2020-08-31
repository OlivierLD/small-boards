// Demo program for testing library and board - flip the switch to turn on/off buzzer

#include <Adafruit_CircuitPlayground.h>

void setup() {
  Serial.begin(9600);
  Serial.println("Circuit Playground test!");

  CircuitPlayground.begin();
}


void loop() {
  // turn off speaker when not in use
  CircuitPlayground.speaker.enable(false);

  /************* TEST CAPTOUCH */
  if (false) {
    Serial.print("Capsense #3: "); Serial.println(CircuitPlayground.readCap(3));
    Serial.print("Capsense #2: "); Serial.println(CircuitPlayground.readCap(2));
    Serial.print("Capsense #0: "); Serial.println(CircuitPlayground.readCap(0));
    Serial.print("Capsense #1: "); Serial.println(CircuitPlayground.readCap(1));
    Serial.print("Capsense #12: "); Serial.println(CircuitPlayground.readCap(12));
    Serial.print("Capsense #6: "); Serial.println(CircuitPlayground.readCap(6));
    Serial.print("Capsense #9: "); Serial.println(CircuitPlayground.readCap(9));
    Serial.print("Capsense #10: "); Serial.println(CircuitPlayground.readCap(10));
  } else {
    boolean atLeastOne = false;
    if (CircuitPlayground.readCap(3) > 500) {
      Serial.print(" A4");
      atLeastOne = true;
    }
    if (CircuitPlayground.readCap(2) > 500) {
      Serial.print(" A5");
      atLeastOne = true;
    }
    if (CircuitPlayground.readCap(0) > 500) {
      Serial.print(" A6");
      atLeastOne = true;
    }
    if (CircuitPlayground.readCap(1) > 500) {
      Serial.print(" A7");
      atLeastOne = true;
    }
    if (CircuitPlayground.readCap(6) > 500) {
      Serial.print(" A1");
      atLeastOne = true;
    }
    if (CircuitPlayground.readCap(9) > 500) {
      Serial.print(" A2");
      atLeastOne = true;
    }
    if (CircuitPlayground.readCap(10) > 500) {
      Serial.print(" A3");
      atLeastOne = true;
    }
    if (atLeastOne) {
      Serial.println();
    }
  }
  delay(10);


}
