#include <Servo.h>

/**
 * Arduino Car
 * Test the Front servo
 * (holding the range sensor)
 */
int servoPin = 11; // Servo signal pin (orange)

Servo servo;          // Set up the servo

int delayMS = 500;    // After the servo motor to the stability of the time

boolean DEBUG = false;

void setup() {

  Serial.begin(9600);     // Initialize
  servo.attach(servoPin);    // Define the servo motor output 10 pin(PWM)
}

void loop() {
  servo.write(90);  /* Make the servo motor ready position Prepare the next measurement */
  delay(delayMS);
  servo.write(170); // 167 ?
  delay(delayMS);
  servo.write(90);  /* Make the servo motor ready position Prepare the next measurement */
  delay(delayMS);
  servo.write(10);  // 15 ?
  delay(delayMS);
}

