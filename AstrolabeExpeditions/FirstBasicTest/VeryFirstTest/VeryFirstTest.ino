int led = 2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Akeu Coucou ! ");
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
}

void loop() {
  Serial.println("Loop top.");

  for (int i=0; i<15; i++) {
    digitalWrite(led, HIGH);
    delay(200);
    digitalWrite(led, LOW);
    delay(200);
  }

  delay(1000);
  // put your main code here, to run repeatedly:

}
