/*
  IR Test Code
  Maxwell Jones
*/

#include <Wire.h>

#define HAPTIC_PIN 1
#define LASER_PIN 2
#define BUTTON_PIN 3
#define IR_PIN 4

#define LASER_DELAY 2000
#define SESSION_TIME 5000
#define HAPTIC_DELAY 2000

unsigned long button_time;
boolean button_on;

void setup() {
  Serial.begin(115200);

  pinMode(LASER_PIN, OUTPUT);
  pinMode(HAPTIC_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(IR_PIN, INPUT);

  button_on = false;
}

void loop() {
  float pulse;
  int sum = 0;
  for (int i = 0; i < 20; i++)
    sum += analogRead(IR_PIN);
  pulse = sum / 20.00;
  Serial.print(900);
  Serial.print(", ");
  Serial.print(650);
  Serial.print(", ");
  Serial.println(pulse);

  if (!button_on && digitalRead(BUTTON_PIN)) {
    button_time = millis();
    button_on = true;
  }
  else if (button_on && !digitalRead(BUTTON_PIN))
  {
    button_on = false;
  }

  if (button_on) {
    if (millis() - button_time > LASER_DELAY+SESSION_TIME+HAPTIC_DELAY)
    {
      Serial.println("Session Done!");
      analogWrite(LASER_PIN, 0);
      digitalWrite(HAPTIC_PIN, LOW);
    }
    else if (millis() - button_time > LASER_DELAY+SESSION_TIME)
    {
      Serial.println("Buzzing!");
      analogWrite(LASER_PIN, 0);
      digitalWrite(HAPTIC_PIN, HIGH);
    }
    else if (millis() - button_time > LASER_DELAY)
    {
      Serial.println("Laser On!");
      analogWrite(LASER_PIN, 150);
    }
    
    
  }

  else {
    analogWrite(LASER_PIN, 0);
    analogWrite(HAPTIC_PIN, 0);
  }

  delay(10);
}
