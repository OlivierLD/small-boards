#include <Servo.h>

#define abs(x) ((x)>0?(x):-(x))

// For HC-SR04
int trigPin = 8;  // Define the ultrasound signal emission Pin TRIG
int echoPin = 9;  // Define the ultrasound signal receiving a Pin ECHO

int servoPin = 11; // Servo signal pin (orange)

Servo servo;          // Set up the servo

int delayMS = 50;    // After the servo motor to the stability of the time

float distance = 0.0;

boolean DEBUG = false;
boolean CONSOLE_OUTPUT = false; // True to see in the Serial Console, false for the Processing client.

int servoOrientation = 90;
int incr = 1;

void setup() {

  Serial.begin(9600);         // Initialize

  pinMode(echoPin, INPUT);    // Define the ultrasound enter pin
  pinMode(trigPin, OUTPUT);   // Define the ultrasound output pin

  servo.attach(servoPin);     // Define the servo motor output pin(PWM)
  servo.write(servoOrientation); // Center
}

void orientationIncr() {
  servoOrientation += incr;
  if (servoOrientation > 180 || servoOrientation < 0) {
    incr *= -1;
    servoOrientation += (2 * incr);
  }
}

void loop() {
  orientationIncr();
  servo.write(servoOrientation);  /* Make the servo motor ready position Prepare the next measurement */
  distance = getDistance();     // Will be time to distance distance (unit: cm)

  if (CONSOLE_OUTPUT) {
    Serial.print("Orientation:");
    Serial.print(servoOrientation);
    Serial.print(", Distance:");
    Serial.print(distance);
    Serial.println(" cm");
  } else {
    char serialBuffer[32];
    int direction = servoOrientation - 90;
//  sprintf(serialBuffer, "%s%02d;%.02f", (direction < 0 ? "-" : "+"), abs(direction), distance);
//  Serial.println(serialBuffer);
    Serial.print(direction < 0 ? "-" : "+");
    Serial.print(abs(direction));
    Serial.print(";");
    Serial.println(distance);
  }
  delay(delayMS);
}

/**
   getDistance, read from the HC-SR04
*/
float getDistance() {
  digitalWrite(trigPin, LOW);               // For low voltage 2 us ultrasonic launch
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);              // Let ultrasonic launch 10 us high voltage, there is at least 10 us
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);               // Maintaining low voltage ultrasonic launch
  float distance = pulseIn(echoPin, HIGH);  // Read the time difference, in microseconds.
  if (DEBUG) {
    Serial.print("Distance is ");
    Serial.print(distance);
    Serial.print(" microsec");
  }
  distance /= (5.8 * 10);                   // Will be time to distance distance (unit: cm)
  if (DEBUG) {
    Serial.print(", becomes ");
    Serial.print(distance);
    Serial.println(" cm");
  }
  return distance;
}

