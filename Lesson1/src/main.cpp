#include <Arduino.h>

void setup() {
  Serial.begin(9600);
  delay(1000);

  goodbye();
  calculate();
}

void loop() {
  // Nothing to do here
}

void goodbye() {
  Serial.println("Goodbye!");
}

void calculate() {
  int a = 7;
  int b = 5;
  int sum = a + b;
  Serial.print("Sum of ");
  Serial.print(a);
  Serial.print(" and ");
  Serial.print(b);
  Serial.print(" is: ");
  Serial.println(sum);
}
