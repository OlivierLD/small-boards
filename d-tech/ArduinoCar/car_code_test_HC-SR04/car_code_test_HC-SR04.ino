/** 
 *  Arduino Car
 *  Test the HC-SR04 
 *  (distace sensor)
 */
int trigPin = 8;  // Define the ultrasound signal emission Pin TRIG
int echoPin = 9;  // Define the ultrasound signal receiving a Pin ECHO

float frontDistance = 0;

boolean DEBUG = false;

void setup() {

  Serial.begin(9600);     // Initialize

  pinMode(echoPin, INPUT);    // Define the ultrasound enter pin
  pinMode(trigPin, OUTPUT);   // Define the ultrasound output pin
}

void loop() {
// Measure the distance ahead
  frontDistance = getDistance();     // A time to distance distance (unit: cm)
  Serial.print("Front distance:");   // The output distance (unit: cm)
  Serial.print(frontDistance);       // According to the distance
  Serial.println(" cm");
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

