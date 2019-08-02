// измерение опорного напряжения

void setup() {
  uartBegin(9600);
  analogPrescaler(128);       // установить предделитель 128 (иначе некорректно измеряет)
}

void loop() {
  // 1100 является константной, которую нужно калибровать (+- лапоть)
  int vcc = (float)1100 * 1023 / (analogRead(INTERNAL));
  uartPrint("vcc: ");
  uartPrintln(vcc); // напряжение в милливольтах
  delay(300);
}
