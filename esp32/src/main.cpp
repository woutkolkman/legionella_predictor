#include <Arduino.h>

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("LED is on");
  delay(1000);
  Serial.println("LED is off");
  delay(1000);
}