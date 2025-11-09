// int led = 2;
#define THE_LED 2

void setup() {
  Serial.begin(115200);
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
