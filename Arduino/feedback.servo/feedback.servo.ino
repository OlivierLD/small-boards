#include <Servo.h> 
 
Servo feedbackServo;  

// Control and feedback pins
const int SERVO_PIN    = 9;
const int FEEDBACK_PIN = A0;

// Calibration values
int minDegrees;
int maxDegrees;
int minFeedback;
int maxFeedback;
int tolerance = 2; // max feedback measurement error

/*
  This function establishes the feedback values for 2 positions of the servo.
  With this information, we can interpolate feedback values for intermediate positions
*/
void calibrate(Servo servo, int analogPin, int minPos, int maxPos) {
  // Move to the minimum position and record the feedback value
  Serial.println("Min pos");
  servo.write(minPos);
  minDegrees = minPos;
  delay(1000); // make sure it has time to get there and settle
  minFeedback = analogRead(analogPin);
  
  // Move to the maximum position and record the feedback value
  Serial.println("Max pos");
  servo.write(maxPos);
  maxDegrees = maxPos;
  delay(2000); // make sure it has time to get there and settle
  maxFeedback = analogRead(analogPin);
}

int getPos(int analogPin) {
  return map(analogRead(analogPin), minFeedback, maxFeedback, minDegrees, maxDegrees);
}
 
void setup() { 
  Serial.begin(9600);
  
  feedbackServo.attach(SERVO_PIN); 
  calibrate(feedbackServo, FEEDBACK_PIN, 20, 160);  // calibrate for the 20-160 degree range
  feedbackServo.detach(); // To free it, can be moved by hand.
  Serial.println("Servo is now free, move it by hand");
} 

void loop() {
  delay(5000);
  int pos = getPos(FEEDBACK_PIN);
  Serial.print("Pos:");
  Serial.println(pos);
}
