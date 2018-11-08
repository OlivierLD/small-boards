#include <Servo.h>

// Directional commands
#define GO_FORWARD 8
#define GO_RIGHT   6
#define GO_LEFT    4
#define GO_BACK    2
#define DONT_MOVE  0

// For the Motor Board
const int pinLeftBackward = A0;   // Define left motor IN4 Pin A0
const int pinLeftForward  = A1;   // Define left motor IN3 Pin A1

const int pinRightBackward = A2;  // Define right motor IN2 Pin A2
const int pinRightForward  = A3;    // Define right motor IN1 Pin A3

// Motor speed adjustment
const int pinLleftEnabled = 5; // Define left motor ENABLE Pin 5 (PWM)
const int pinRightEnabled = 6; // Define right motor ENABLE Pin 6 (PWM)

// For HC-SR04
const int trigPin = 8;  // Define the ultrasound signal emission Pin TRIG
const int echoPin = 9;  // Define the ultrasound signal receiving a Pin ECHO

//const int leftBumperPin = 4;
//const int rightBumperPin = 7;

// Range sensor orientation
const int servoPin = 11; // Servo signal pin (orange)

const int turnSpeed    = 100;
const int forwardSpeed = 120;
const int backSpeed    = 100;

int direction = DONT_MOVE;

Servo servo;          // Set up the servo

const int delayMS = 500;    // After the servo motor to the stability of the time

const float MIN_FRONT_DIST_1 = 30;
const float MIN_FRONT_DIST_2 = 15;

float frontDistance = 0;
float leftDistance  = 0;
float rightDistance = 0;

const boolean DEBUG = false;

void setup() {

  Serial.begin(9600);     // Initialize

  pinMode(pinLeftBackward, OUTPUT);  // Define A0 pin for the output (PWM)
  pinMode(pinLeftForward, OUTPUT);   // Define A1 pin for the output (PWM)
  pinMode(pinRightBackward, OUTPUT); // Define A2 pin for the output (PWM)
  pinMode(pinRightForward, OUTPUT);  // Define A3 pin for the output (PWM)

  pinMode(pinLleftEnabled,  OUTPUT);  // Define 5 pin for PWM output (connect the back pins of the jumpers)
  pinMode(pinRightEnabled,  OUTPUT);  // Define 6 pin for PWM output (connect the back pins of the jumpers)

  pinMode(echoPin, INPUT);    // Define the ultrasound enter pin
  pinMode(trigPin, OUTPUT);   // Define the ultrasound output pin

  //  pinMode(leftBumperPin, INPUT);    // Define the bumper pin
  //  pinMode(rightBumperPin, INPUT);    // Define the bumper pin

  servo.attach(servoPin);    // Define the servo motor output 10 pin(PWM)
}

void loop() {
  servo.write(90);  /* Make the servo motor ready position Prepare the next measurement */
  detect();           // Measuring Angle And determine which direction to go to

  if (direction == GO_BACK) {
    back(500);                    // back
    //turnLeft(100);              // Move slightly to the left (to prevent stuck in dead end lane)
    Serial.print(" Reverse ");    // According to the direction (reverse)
  } else if (direction == GO_RIGHT) {
    //back(100);
    turnRight(350);               // right
    Serial.print(" Right ");      // According to the direction (Right)
  } else if (direction == GO_LEFT) {
    //back(100);
    turnLeft(350);                // left
    Serial.print(" Left ");       // According to the direction (Left)
  } else if (direction == GO_FORWARD) {
    moveForward();                // go
    Serial.print(" Forward ");    // According to the direction (Advance)
    Serial.print("   ");
  }
  Serial.println();
}

// go
void moveForward() {
  digitalWrite(pinRightBackward, HIGH);  // 16 feet for high level
  digitalWrite(pinRightForward, LOW);    // 17 feet for low level
  analogWrite(pinRightEnabled, forwardSpeed); // Set the output speed(PWM)
  digitalWrite(pinLeftBackward, HIGH);   // 14 feet for high level
  digitalWrite(pinLeftForward, LOW);     // 15 feet for high level
  analogWrite(pinLleftEnabled, forwardSpeed); // Set the output speed(PWM)
}

/**
   go
   @param a in millis
*/
void moveForward(int a) {
  moveForward();
  delay(a);
  stopMoving();
}

/**
   right
   @param d in millis
*/
void turnRight(int d) {
  digitalWrite(pinRightBackward, HIGH);
  digitalWrite(pinRightForward, LOW);
  analogWrite(pinRightEnabled, turnSpeed);
  digitalWrite(pinLeftBackward, LOW);
  digitalWrite(pinLeftForward, HIGH);
  analogWrite(pinLleftEnabled, turnSpeed);
  delay(d);
  stopMoving();
}

/**
   left
   @param e in millis
*/
void turnLeft(int e) {
  digitalWrite(pinRightBackward, LOW);
  digitalWrite(pinRightForward, HIGH);
  analogWrite(pinRightEnabled, turnSpeed);
  digitalWrite(pinLeftBackward, HIGH);
  digitalWrite(pinLeftForward, LOW);
  analogWrite(pinLleftEnabled, turnSpeed);
  delay(e);
  stopMoving();
}

// stop
void stopMoving() {
  digitalWrite(pinRightBackward, LOW);
  digitalWrite(pinRightForward, LOW);
  digitalWrite(pinLeftBackward, LOW);
  digitalWrite(pinLeftForward, LOW);
}

/**
   back
   @param g in millis
*/
void back(int g) {
  digitalWrite(pinRightBackward, LOW);
  digitalWrite(pinRightForward, HIGH);
  analogWrite(pinRightEnabled, backSpeed);
  digitalWrite(pinLeftBackward, LOW);
  digitalWrite(pinLeftForward, HIGH);
  analogWrite(pinLleftEnabled, backSpeed);
  delay(g);
  stopMoving();
}

// Measuring three angles(0.90.179)
void detect() {
  checkFront();            // Read in front of the distance

  //  if (digitalRead(rightBumperPin) == LOW) {
  //    back(500);
  //    turnLeft(350);
  //  } else if (digitalRead(leftBumperPin) == LOW) {
  //    back(500);
  //    turnRight(350);
  //  }

  if (frontDistance < MIN_FRONT_DIST_2) {   // If the front distance less than 15 cm, definitely NOT enough room
    back(200);                              // Then back two milliseconds
  }

  if (frontDistance < MIN_FRONT_DIST_1) {   // If the front distance less than 30 cm
    stopMoving();                           // Remove the output data
    checkLeft();                            // Read the left distance

    servo.write(90);
    delay(delayMS);           // Waiting for the servo motor to be stable

    checkRight();             // Read the right distance

    if (leftDistance < MIN_FRONT_DIST_2 && 
        rightDistance < MIN_FRONT_DIST_2) {  // If the left front distance and distance and the right distance is less than 15 cm
      direction = GO_BACK;      // Move backwards
    } else if (leftDistance > rightDistance) { // If the distance is greater than the right distance on the left
      direction = GO_LEFT;      // Left
    } else if (leftDistance <= rightDistance) { // If the distance is less than or equal to the distance on the right
      direction = GO_RIGHT;     // Right
    }
  } else {                      // If the front is not less than 25 cm (greater than)
    direction = GO_FORWARD;     // Move forward
  }

  servo.write(90); // In the center
  //delay(delayMS);
}

// Measure the distance ahead
void checkFront() {
  servo.write(90);
  if (direction != GO_FORWARD) {
    delay(delayMS);
//} else {
//  delay(10);
  }
  frontDistance = getDistance();     // A time to distance distance (unit: cm)
  Serial.print("Front distance:");   // The output distance (unit: cm)
  Serial.println(frontDistance);     // According to the distance
}

// Measure the distance on the left
void checkLeft()  {
  servo.write(165); // 180 - 15
  delay(delayMS);

  leftDistance = getDistance();      // Will be time to distance distance (unit: cm)
  Serial.print("Left distance:");    // The output distance (unit: cm)
  Serial.println(leftDistance);      // According to the distance
}

void checkRight() {                  // Measure the distance on the right
  servo.write(15);  
  delay(delayMS);

  rightDistance = getDistance();     // Will be time to distance distance (unit: cm)
  Serial.print("Right distance:");   // The output distance (unit: cm)
  Serial.println(rightDistance);     // According to the distance
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
    Serial.print("Elapsed time is ");
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

