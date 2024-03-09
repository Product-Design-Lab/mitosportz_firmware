/*
  Test Code for the 3 Cell Breadboard Prototype
  Maxwell Jones
*/

#include <Wire.h>

/* Pin Definitions */
const int CLOCK_PIN = 0;
const int LATCH_PIN = 1;
const int DATA_PIN = 2;
const int PULSE_PIN = 3;
const int VOLTAGE_MOD_PIN = 7;
const int BUTTON_PIN = 9;
const int POWER_PIN = 10;

// const int SCOPE_PIN = 5;

/* Global Constants */
const unsigned int PULSE_DUTY_CYCLE = 50; // % duty cycle
const unsigned long VCSEL1_LOOP_TIME = 250;
const unsigned long VCSEL2_LOOP_TIME = 500;
const unsigned long VCSEL3_LOOP_TIME = 1000;
const unsigned long DEBOUNCE_DELAY = 500;
const unsigned long HAPTIC_ON_TIME = 250;

/* Global Variables*/
unsigned long vcsel1_time;
unsigned long vcsel2_time;
unsigned long vcsel3_time;
boolean vcsel1_state;
boolean vcsel2_state;
boolean vcsel3_state;
boolean led1_state;
boolean led2_state;
boolean led3_state;
boolean status1_state;
boolean status2_state;
boolean status3_state;
boolean status4_state;
boolean power_state;
unsigned long debounce_time;
unsigned long haptic_time;
boolean haptic_state;
boolean contact_state;

/* Setup Function */
void setup() {
  Serial.begin(115200);

  // Output pins
  pinMode(PULSE_PIN, OUTPUT);
  pinMode(VOLTAGE_MOD_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(POWER_PIN, OUTPUT);

  // Input pins
  // pinMode(SCOPE_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);

  // Initialise global variables
  vcsel1_time = millis();
  vcsel2_time = millis();
  vcsel3_time = millis();
  vcsel1_state = false;
  vcsel2_state = false;
  vcsel3_state = false;
  led1_state = false;
  led2_state = false;
  led3_state = false;
  status1_state = false;
  status2_state = false;
  status3_state = false;
  status4_state = false;
  power_state = false;
  debounce_time = millis();
  haptic_time = millis();
  haptic_state = false;
  contact_state = false;

  // Setup I2C for GPIO Expander MCP23008 for all inputs
  // Wire.begin();
  // Wire.beginTransmission(0x20);
  // Wire.write(0x00);
  // Wire.write(0xFF);
  // Wire.endTransmission();

  // Create pulse signal for VCSELs 
  // (still need to edit registers for exact freq.)
  analogWrite(PULSE_PIN, floor((PULSE_DUTY_CYCLE * 255)/100));

  // Power starts off
  digitalWrite(POWER_PIN, LOW);
}

/* Loop Function */
void loop() {
  // Read inputs from expander
  // Wire.beginTransmission(0x20);
  // Wire.write(0x09);
  // Wire.endTransmission();
  // Wire.requestFrom(0x20, 1);
  // byte expander_input = Wire.read();

  // if (!haptic_state && expander_input == B00000001) {
  //   haptic_state = true;
  //   haptic_time = millis();
  // }

  if ((millis() - debounce_time > DEBOUNCE_DELAY) && digitalRead(BUTTON_PIN)) {
    if (!power_state) {
      power_state = true;
      digitalWrite(POWER_PIN, HIGH);
    } else {
      power_state = false;
      digitalWrite(POWER_PIN, LOW);
    }

    debounce_time = millis();
  }

  // Voltage mod output for VCSELs
  digitalWrite(VOLTAGE_MOD_PIN, HIGH);

  // VCSEL pattern
  if (millis() - vcsel1_time >= VCSEL1_LOOP_TIME) {
    vcsel1_state = !vcsel1_state;
    led1_state = !led1_state;
    vcsel1_time = millis();
  }

  if (millis() - vcsel2_time >= VCSEL2_LOOP_TIME) {
    vcsel2_state = !vcsel2_state;
    led2_state = !led2_state;
    vcsel2_time = millis();
  }

  if (millis() - vcsel3_time >= VCSEL3_LOOP_TIME) {
    vcsel3_state = !vcsel3_state;
    led3_state = !led3_state;
    vcsel3_time = millis();
  }

  // Use pin for voltage scope as temp measure
  // int scope_value = analogRead(SCOPE_PIN);
  // float scope_voltage = scope_value*(5.0/1023.0);
  // Serial.print(scope_voltage);
  // Serial.print(", ");
  // Serial.print(0);
  // Serial.print(", ");
  // Serial.println(3.3);

  status1_state = true;

  if ((vcsel1_state?1:0)+(vcsel2_state?1:0)+(vcsel3_state?1:0) == 3) {
    status2_state = true;
    status3_state = true;
    status4_state = true;
  } else if ((vcsel1_state?1:0)+(vcsel2_state?1:0)+(vcsel3_state?1:0) == 2) {
    status2_state = true;
    status3_state = true;
    status4_state = false;
  }
  else if ((vcsel1_state?1:0)+(vcsel2_state?1:0)+(vcsel3_state?1:0) == 1) {
    status2_state = true;
    status3_state = false;
    status4_state = false;
  } else {
    status2_state = false;
    status3_state = false;
    status4_state = false;
  }

  if (haptic_state && (millis() - haptic_time >= HAPTIC_ON_TIME)) {
    haptic_state = false;
  }

  // Send data to shift register
  int dataA_bits[8] = {0,0,led3_state,led2_state,led1_state,vcsel3_state,vcsel2_state,vcsel1_state};
  int dataB_bits[8] = {0,0,haptic_state,status4_state,status3_state,status2_state,status1_state,0};
  byte dataA_byte = 0;
  byte dataB_byte = 0;

  for (int i = 0; i < 8; i++){
    if (dataA_bits[i]) {
      dataA_byte |= (1 << (7-i));
    }
  }

  for (int i = 0; i < 8; i++){
    if (dataB_bits[i]) {
      dataB_byte |= (1 << (7-i));
    }
  }


  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, dataB_byte);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, dataA_byte);
  digitalWrite(LATCH_PIN, HIGH);

  // delay(5);
}
