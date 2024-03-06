

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
BLEIntCharacteristic batteryLevelCharacteristic("20B10021-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify); // The battery level of the device from 0-100
BLEIntCharacteristic heartRateCharacteristic("20B10022-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify); // Heart rate
BLEIntCharacteristic bloodOxygenCharacteristic("20B10023-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify); // Pulse Oximetry
BLEIntCharacteristic sessionDurationCharacteristic("20B10024-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify); // LED On Time
BLEIntCharacteristic activeLaserProportionCharacteristic("20B10025-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify); // Proportion of active lasers
BLEIntCharacteristic laserPowerLevelCharacteristic("20B10026-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify); // Percentage of maximum laser power
BLEByteCharacteristic ledTimingCharacteristic("20B10027-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify); // Laser Diode Timing
BLEByteCharacteristic laserTimingCharacteristic("20B10028-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify); // Laser Diode Delay
BLEByteCharacteristic resetCharacteristic("20B10029-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify); // Resets the device and the app


// Misc
int i = 0;
bool usbConnected = false;

int batteryLevel = 100;
int heartRate = 65;
int bloodOxygenSaturation = 100;
int sessionDuration = 560;
int activeLaserProportion = 50;
int laserPowerLevel = 50;

int ledTiming = 0;
int laserTiming = 0;

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


  BLE.setDeviceName("Mitosports_01_B");   // set the device name
  BLE.setLocalName("Mitosports_01_B");    // set the local name peripheral advertises
  BLE.setAdvertisedService(mitosportzService); // set the UUID for the service this peripheral advertises:
  mitosportzService.addCharacteristic(batteryLevelCharacteristic); 
  mitosportzService.addCharacteristic(heartRateCharacteristic); 
  mitosportzService.addCharacteristic(bloodOxygenCharacteristic); 
  mitosportzService.addCharacteristic(sessionDurationCharacteristic); 
  mitosportzService.addCharacteristic(activeLaserProportionCharacteristic); 
  mitosportzService.addCharacteristic(laserPowerLevelCharacteristic); 
  mitosportzService.addCharacteristic(ledTimingCharacteristic); 
  mitosportzService.addCharacteristic(laserTimingCharacteristic); 
  mitosportzService.addCharacteristic(resetCharacteristic); 
  BLE.addService(mitosportzService);

  sessionDurationCharacteristic.setEventHandler(BLEWritten, sessionDurationCharacteristicWritten);
  activeLaserProportionCharacteristic.setEventHandler(BLEWritten, activeLaserProportionCharacteristicWritten);
  laserPowerLevelCharacteristic.setEventHandler(BLEWritten, laserPowerLevelCharacteristicWritten);
  ledTimingCharacteristic.setEventHandler(BLEWritten, sessionDurationCharacteristicWritten);
  laserTimingCharacteristic.setEventHandler(BLEWritten, sessionDurationCharacteristicWritten);
  resetCharacteristic.setEventHandler(BLEWritten, resetCharacteristicWritten);

  BLE.advertise(); // start advertising

  i = 0;   // Reset arbitrary counter

}

void loop() {

  if (i % 15 == 0) {
    batteryLevel--;
    laserPowerLevel--;
    sessionDuration--;
  }
  if (batteryLevel < 0) {
    batteryLevel = 100;
    laserPowerLevel = 100;
    sessionDuration = 560;
  }

  if (i % 2 == 0) {
    activeLaserProportion = 50;
  } else {
    activeLaserProportion = 100;
  }

  heartRate = 5*sin(i*PI/16) + 45;
  bloodOxygenSaturation = 5*sin(i*PI/16) + 80;

  
  BLE.poll();

  // TEST: Set Value - Is this required for BLENotify?
  batteryLevelCharacteristic.setValue(batteryLevel);
  heartRateCharacteristic.setValue(heartRate);
  bloodOxygenCharacteristic.setValue(bloodOxygenSaturation);
  sessionDurationCharacteristic.setValue(sessionDuration);
  activeLaserProportionCharacteristic.setValue(activeLaserProportion);
  laserPowerLevelCharacteristic.setValue(laserPowerLevel);
  ledTimingCharacteristic.setValue(ledTiming);
  laserTimingCharacteristic.setValue(laserTiming);
  
  // Write Value
  batteryLevelCharacteristic.writeValue(batteryLevel);
  heartRateCharacteristic.writeValue(heartRate);
  bloodOxygenCharacteristic.writeValue(bloodOxygenSaturation);
  sessionDurationCharacteristic.writeValue(sessionDuration);
  activeLaserProportionCharacteristic.writeValue(activeLaserProportion);
  laserPowerLevelCharacteristic.writeValue(laserPowerLevel);
  ledTimingCharacteristic.writeValue(ledTiming);
  laserTimingCharacteristic.writeValue(laserTiming);
  
  
  Serial.println("Device B - Heart Rate: " + String(heartRate) + "bpm, Blood Oxygen Saturation: " + String(bloodOxygenSaturation) + "%, Session Duration: " + String(sessionDuration) + "s, Active Laser Proportion: " + String(activeLaserProportion) + "%, Laser Power Level: " + String(laserPowerLevel) + "%");

  delay(50);
  i++;

}



void sessionDurationCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {

  Serial.println("Received sessionDuration: " + String(characteristic.value()[0]));
  sessionDuration = characteristic.value()[0];


  for (int j=0; j < 10; j++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }

}

void activeLaserProportionCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {

  Serial.println("Received activeLaserProportion: " + String(characteristic.value()[0]));
  activeLaserProportion = characteristic.value()[0];


  for (int j=0; j < 10; j++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }

}

void laserPowerLevelCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {

  Serial.println("Received laserPowerLevel: " + String(characteristic.value()[0]));
  laserPowerLevel = characteristic.value()[0];


  for (int j=0; j < 10; j++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }

}

void ledTimingCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {

  Serial.println("Received ledTiming: " + String(characteristic.value()[0]));
  ledTiming  = characteristic.value()[0];


  for (int j=0; j < 10; j++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }

}

void laserTimingCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {

  Serial.println("Received laserTiming: " + String(characteristic.value()[0]));
  laserTiming  = characteristic.value()[0];

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