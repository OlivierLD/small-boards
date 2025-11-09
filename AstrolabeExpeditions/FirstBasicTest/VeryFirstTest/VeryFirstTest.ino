// int led = 2;
#define THE_LED 2

void setup() {
  Serial.begin(115200);  // Make sure this matches the console's speed (aka baud rate).
  // Serial.begin(9600); // 115200 may produce un-readable characters... 9600 is the default speed for the Serial Console...
  while (!Serial);
  Serial.println("Akeu Coucou ! ");
  pinMode(THE_LED, OUTPUT);
  digitalWrite(THE_LED, LOW);
}

void loop() {
  Serial.println("Loop top.");

  for (int i=0; i<15; i++) {
    digitalWrite(THE_LED, HIGH);
    delay(200);
    digitalWrite(THE_LED, LOW);
    delay(200);
  }

  delay(1000);
  // put your main code here, to run repeatedly...

}