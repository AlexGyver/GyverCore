// скетч для проверки занимаемой памяти

void setup() {
  pinMode(2, 1);
  digitalWrite(1, 1);
  digitalRead(0);
  analogRead(0);
  analogReference(DEFAULT);
  analogWrite(9, 125);
  millis();
  micros();
  delay(10);
  delayMicroseconds(10);
  tone(10, 10);
  tone(10, 10, 100);
  attachInterrupt(0, isr, 0);
  detachInterrupt(0);
  //uartBegin();
  //uartPrintln("kek OK");
  Serial.begin(9600);
  Serial.println("kek OK");
}
void isr(){}

void loop() {
  // put your main code here, to run repeatedly:

}
