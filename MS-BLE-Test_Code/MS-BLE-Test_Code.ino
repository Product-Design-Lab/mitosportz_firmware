

#include <ArduinoBLE.h>
#include <math.h>
#define SERIAL_TIMEOUT 4000

// Timers 
unsigned long tick = 0;
unsigned long tock = 0;
unsigned long timer = 0;

// Pins
const int ledPin = 13; 

// Bluetooth Services
BLEService mitosportzService("20B10020-E8F2-537E-4F6C-D104768A1214"); // create service

// Bluetooth Characteristics
BLEIntCharacteristic heartRateCharacteristic("20B10022-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify); // Heart rate
BLEIntCharacteristic bloodOxygenCharacteristic("20B10021-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify); // Pulse Oximetry
BLEIntCharacteristic batteryLevelCharacteristic("20B10022-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify); // The battery level of the device from 0-100

BLEByteCharacteristic resetCharacteristic("20B10023-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify); // Resets the device and the app
BLEByteCharacteristic LEDOnTimeCharacteristic("20B10024-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify); // LED On Time
BLEByteCharacteristic LaserDiodeTimingCharacteristic("20B10025-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify); // Laser Diode Timing
BLEByteCharacteristic LaserDiodeDelayCharacteristic("20B10026-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify); // Laser Diode Delay


// Misc
int i = 0;
bool usbConnected = false;

int batteryLevel = 100;
int heartRate = 65;
int bloodOxygenSaturation = 100;

int LEDOnTime = 0;
int LaserDiodeTiming = 0;
int LaserDiodeDelay = 0;

void setup() {
  Serial.begin(115200);   // Set serial baud rate for USB 
  tick = millis();        // Start Timer
  while(!Serial) {        // Wait For Serial To Connect 
    if ((millis() - tick) > SERIAL_TIMEOUT) {
      usbConnected = false;
      break;
    }
  }


  if (!BLE.begin()) {     // Enable Bluetooth
    Serial.println("BLE Failed");
    if (usbConnected) {
      Serial.println("starting Bluetooth® Low Energy module failed!");
    }
    Serial.println("Entering LED Error State");
    errorLED();
  } 


  BLE.setDeviceName("Mitosports_01");   // set the device name
  BLE.setLocalName("Mitosports_01");    // set the local name peripheral advertises
  BLE.setAdvertisedService(mitosportzService); // set the UUID for the service this peripheral advertises:
  mitosportzService.addCharacteristic(heartRateCharacteristic); 
  mitosportzService.addCharacteristic(batteryLevelCharacteristic); 
  mitosportzService.addCharacteristic(bloodOxygenCharacteristic); 
  mitosportzService.addCharacteristic(resetCharacteristic); 
  mitosportzService.addCharacteristic(LEDOnTimeCharacteristic); 
  mitosportzService.addCharacteristic(LaserDiodeTimingCharacteristic); 
  mitosportzService.addCharacteristic(LaserDiodeDelayCharacteristic); 
  BLE.addService(mitosportzService);

  LEDOnTimeCharacteristic.setEventHandler(BLEWritten, LEDOnTimeCharacteristicWritten);
  LaserDiodeTimingCharacteristic.setEventHandler(BLEWritten, LEDOnTimeCharacteristicWritten);
  LaserDiodeDelayCharacteristic.setEventHandler(BLEWritten, LEDOnTimeCharacteristicWritten);
  resetCharacteristic.setEventHandler(BLEWritten, resetCharacteristicWritten);

  BLE.advertise(); // start advertising

  i = 0;   // Reset arbitrary counter

}

void loop() {

  if (i % 15 == 0) {
    batteryLevel--;
  }
  if (batteryLevel < 0) {
    batteryLevel = 100;
  }


  heartRate = 5*sin(i*PI/16) + 65;
  bloodOxygenSaturation = 5*sin(i*PI/16) + 95;

  
  BLE.poll();

  // TEST: Set Value - Is this required for BLENotify?
  heartRateCharacteristic.setValue(heartRate);
  bloodOxygenCharacteristic.setValue(bloodOxygenSaturation);
  batteryLevelCharacteristic.setValue(batteryLevel);
  LEDOnTimeCharacteristic.setValue(LEDOnTime);
  LaserDiodeTimingCharacteristic.setValue(LaserDiodeTiming);
  LaserDiodeDelayCharacteristic.setValue(LaserDiodeDelay);
  
  // Write Value
  heartRateCharacteristic.writeValue(heartRate);
  bloodOxygenCharacteristic.writeValue(bloodOxygenSaturation);
  batteryLevelCharacteristic.writeValue(batteryLevel);
  LEDOnTimeCharacteristic.writeValue(LEDOnTime);
  LaserDiodeTimingCharacteristic.writeValue(LaserDiodeTiming);
  LaserDiodeDelayCharacteristic.writeValue(LaserDiodeDelay);
  
  
  Serial.println("Heart Rate: " + String(heartRate) + "bpm, Blood Oxygen Saturation: " + String(bloodOxygenSaturation) + "%");

  delay(50);
  i++;

}



void LEDOnTimeCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {

  Serial.println("Received LEDOnTime: " + String(characteristic.value()[0]));
  LEDOnTime = characteristic.value()[0];


  for (int j=0; j < 10; j++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }

}
void LaserDiodeTimingCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {

  Serial.println("Received LaserDiodeTiming: " + String(characteristic.value()[0]));
  LaserDiodeTiming  = characteristic.value()[0];


  for (int j=0; j < 10; j++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }

}

void LaserDiodeDelayCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {

  Serial.println("Received LaserDiodeDelay: " + String(characteristic.value()[0]));
  LaserDiodeDelay  = characteristic.value()[0];

  for (int j=0; j < 10; j++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }

}

void resetCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  
  Serial.println("Reset");
  i = 0;

  for (int j=0; j < 10; j++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }

}


void errorLED() {

  if (usbConnected) {
    Serial.println("ERROR");
  }

  while(1) {
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
  }
}