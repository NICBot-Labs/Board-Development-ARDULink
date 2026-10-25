const int lm35Pin = A1;

void setup() {
  Serial.begin(115200);
}

void loop() {
  int analogValue = analogRead(lm35Pin);

  float voltage = analogValue * (5.0 / 1023.0);
  float temperatureC = voltage * 100.0;  

  Serial.print("ADC: ");
  Serial.print(analogValue);
  Serial.print(" | Tegangan: ");
  Serial.print(voltage, 3);
  Serial.print(" V | Suhu: ");
  Serial.print(temperatureC, 2);
  Serial.println(" °C");

  delay(1000);
}