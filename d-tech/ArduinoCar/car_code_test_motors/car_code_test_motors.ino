/** 
 *  Arduino Car
 *  Test the Motor Board
 */
int pinLeftBackward = A0;   // Define left motor IN4 Pin A0
int pinLeftForward  = A1;   // Define left motor IN3 Pin A1

int pinRightBackward = A2;  // Define right motor IN2 Pin A2
int pinRightForward  = A3;    // Define right motor IN1 Pin A3

int turnSpeed = 250;
int forwardSpeed = 180;
int backSpeed = 180;

int delayMS = 500;    // After the servo motor to the stability of the time

boolean DEBUG = false;

void setup() {

  Serial.begin(9600);     // Initialize

  pinMode(pinLeftBackward, OUTPUT);  // Define A0 pin for the output (PWM)
  pinMode(pinLeftForward, OUTPUT);   // Define A1 pin for the output (PWM)
  pinMode(pinRightBackward, OUTPUT); // Define A2 pin for the output (PWM)
  pinMode(pinRightForward, OUTPUT);  // Define A3 pin for the output (PWM)
}

// go
void moveForward() {
  digitalWrite(pinRightBackward, HIGH);  // 16 feet for high level
  digitalWrite(pinRightForward, LOW);    // 17 feet for low level
  // analogWrite(pinRENA, forwardSpeed); // Set the output speed(PWM)
  digitalWrite(pinLeftBackward, HIGH);   // 14 feet for high level
  digitalWrite(pinLeftForward, LOW);     // 15 feet for high level
  // analogWrite(pinLENA, forwardSpeed); // Set the output speed(PWM)
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
  // analogWrite(pinRENA, turnSpeed);
  digitalWrite(pinLeftBackward, LOW);
  digitalWrite(pinLeftForward, HIGH);
  // analogWrite(pinLENA, turnSpeed);
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
  // analogWrite(pinRENA, turnSpeed);
  digitalWrite(pinLeftBackward, HIGH);
  digitalWrite(pinLeftForward, LOW);
  // analogWrite(pinLENA, turnSpeed);
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
  // analogWrite(pinRENA, backSpeed);
  digitalWrite(pinLeftBackward, LOW);
  digitalWrite(pinLeftForward, HIGH);
  // analogWrite(pinLENA, backSpeed);
  delay(g);
  stopMoving();
}

void loop() {

   // Right motor
  Serial.println(" Right motor, forward");
  digitalWrite(pinRightBackward, LOW);
  digitalWrite(pinRightForward, HIGH);
  delay(1000);
  // Stop
  digitalWrite(pinRightBackward, LOW);
  digitalWrite(pinRightForward, LOW);

  Serial.println(" Right motor, backward");
  digitalWrite(pinRightForward, LOW);
  digitalWrite(pinRightBackward, HIGH);
  delay(1000);
  // Stop
  digitalWrite(pinRightBackward, LOW);
  digitalWrite(pinRightForward, LOW);

   // Left motor
  Serial.println(" Left motor, forward");
  digitalWrite(pinLeftBackward, LOW);
  digitalWrite(pinLeftForward, HIGH);
  delay(1000);
  // Stop
  digitalWrite(pinLeftBackward, LOW);
  digitalWrite(pinLeftForward, LOW);

  Serial.println(" Right motor, backward");
  digitalWrite(pinLeftForward, LOW);
  digitalWrite(pinLeftBackward, HIGH);
  delay(1000);
  // Stop
  digitalWrite(pinLeftBackward, LOW);
  digitalWrite(pinLeftForward, LOW);

  // Both motors
  Serial.println(" Reverse ");    // According to the direction (reverse)
  back(500);                      // back
  delay(delayMS);
  Serial.println(" Right ");      // According to the direction (Right)
  turnRight(350);                 // right
  delay(delayMS);
  Serial.println(" Left ");       // According to the direction (Left)
  turnLeft(350);                  // left
  delay(delayMS);
  Serial.println(" Forward ");    // According to the direction (Advance)
  moveForward();                  // go
  delay(delayMS);
}

