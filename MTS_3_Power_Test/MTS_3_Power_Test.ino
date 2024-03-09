/*
  Test Code for checking optical power of each VCSEL
  Maxwell Jones
*/

#include <Wire.h>

/* Pin Definitions */
const int SCOPE_PIN = 0;
const int PULSE_PIN = 3;
const int VOLTAGE_MOD_PIN = 7;

/* Global Constants */
const unsigned int PULSE_DUTY_CYCLE = 50; // % duty cycle

/* Global Variables*/

/* Setup Function */
void setup() {
  Serial.begin(115200);

  // Output pins
  pinMode(PULSE_PIN, OUTPUT);
  pinMode(VOLTAGE_MOD_PIN, OUTPUT);

  // Input pins
  pinMode(SCOPE_PIN, INPUT);

  // Create pulse signal for VCSELs
  digitalWrite(VOLTAGE_MOD_PIN, HIGH);
  // (still need to edit registers for exact freq.)
  analogWrite(PULSE_PIN, floor((PULSE_DUTY_CYCLE * 255)/100));
}

/* Loop Function */
void loop() {
  // Use pin for voltage scope as temp measure
  int scope_value = analogRead(SCOPE_PIN);
  float scope_voltage = scope_value*(5.0/1023.0);
  Serial.print(scope_voltage);
  Serial.print(", ");
  Serial.print(0);
  Serial.print(", ");
  Serial.println(3.3);

  // delay(10);
}
