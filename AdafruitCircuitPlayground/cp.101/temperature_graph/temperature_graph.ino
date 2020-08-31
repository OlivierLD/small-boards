#include <Adafruit_CircuitPlayground.h>

/**
 * Disply using the Graph (Tools > Serial Plotter)
 */
float tempC, tempF;

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
}

void loop() {
  tempC = CircuitPlayground.temperature();
  tempF = CircuitPlayground.temperatureF();

  Serial.print("tempC: ");
  Serial.print(tempC);
  Serial.print("  tempF: ");
  Serial.println(tempF);

  delay(100);
}
