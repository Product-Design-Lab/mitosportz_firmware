

#include <ArduinoBLE.h>
#define SERIAL_TIMEOUT 4000

// Timers
unsigned long tick = 0;
unsigned long tock = 0;
unsigned long timer = 0;

// Pins
const int ledPin = 13;

// Bluetooth Services
BLEService mitosportzService("20B10020-E8F2-537E-4F6C-D104768A1214");

// Bluetooth Characteristics
BLEIntCharacteristic batteryLevelCharacteristic("20B10021-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify);
BLEStringCharacteristic testCharacteristic("20B10030-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 9);

// Misc
bool usbConnected = false;

int i = 0;
int batteryLevel = 100;
char test[9] = "01000000";

void setup() {

  Serial.begin(115200);
  tick = millis();
  while (!Serial) {
    if ((millis() - tick) > SERIAL_TIMEOUT) {
      usbConnected = false;
      break;
    }
  }

  if (!BLE.begin()) {
    Serial.println("BLE Failed");
    if (usbConnected) {
      Serial.println("starting Bluetooth® Low Energy module failed!");
    }
    Serial.println("Entering LED Error State");
  }

  BLE.setDeviceName("Mitosports_01_A");
  BLE.setLocalName("Mitosports_01_A");
  BLE.setAdvertisedService(mitosportzService);

  mitosportzService.addCharacteristic(batteryLevelCharacteristic);
  mitosportzService.addCharacteristic(testCharacteristic);

  BLE.addService(mitosportzService);

  testCharacteristic.setEventHandler(BLEWritten, testCharacteristicWritten);

  BLE.advertise();

  i = 0;
}

void loop() {

  if (i % 15 == 0) {
    batteryLevel--;
  }
  if (batteryLevel < 0) {
    batteryLevel = 100;
  }

  if (batteryLevel > 50) {
    test[0] = '1';
  } else {
    test[0] = '0';
  }

  BLE.poll();

  batteryLevelCharacteristic.setValue(batteryLevel);
  testCharacteristic.setValue(test);

  Serial.println("BATTERY LEVEL: " + String(batteryLevel) + " STRING: " + String(test));

  delay(50);
  i++;
}

void testCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  Serial.println("RECEIVED TEST CHARACTERISTIC");
  test[0] = characteristic.value()[0];

  for (int j=0; j < 9; j++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}
