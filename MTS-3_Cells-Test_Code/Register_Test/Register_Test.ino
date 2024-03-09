/*
  Test Code for the 3 Cell Breadboard Prototype
  Maxwell Jones
*/

#include <Wire.h>

#define PULSE_PIN A3
#define SCOPE_PIN A1

#define GPIO_P0_BASE_ADDRESS = 0x50000000
#define OUTSET_OFFSET = 0x508;
#define OUTCLR_OFFSET = 0x50C;
#define PIN29_OFFSET = 0x01D;

volatile unsigned long * GPIO_OUTSET_PIN29_reg = (volatile unsigned long *)(GPIO_P0_BASE_ADDRESS+OUTSET_OFFSET+PIN29_OFFSET);
volatile unsigned long * GPIO_OUTCLR_PIN29_reg = (volatile unsigned long *)(GPIO_P0_BASE_ADDRESS+OUTCLR_OFFSET+PIN29_OFFSET);

void setup() {
  Serial.begin(115200);

  pinMode(PULSE_PIN, OUTPUT);
  pinMode(SCOPE_PIN, INPUT);

  digitalWrite(PULSE_PIN, LOW);

}

void loop() {
  // digitalWrite(PULSE_PIN, HIGH);

  // int scope_value = analogRead(SCOPE_PIN);
  // float scope_voltage = scope_value*(5.0/1023.0);

  // Serial.println(scope_voltage);

  // delay(200);

  // digitalWrite(PULSE_PIN, LOW);

  // scope_value = analogRead(SCOPE_PIN);
  // scope_voltage = scope_value*(5.0/1023.0);

  // Serial.println(scope_voltage);

  // delay(200);

  *GPIO_OUTSET_PIN29_reg = GPIO_OUTSET_PIN29_Set;
  delay(500);
  *GPIO_OUTCLR_PIN29_reg = GPIO_OUTCLR_PIN29_Clear;
  delay(500);

}
