

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
BLEIntCharacteristic batteryLevelChar("20B10021-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify);
BLEIntCharacteristic sessionDurationChar("20B10022-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify);

BLEStringCharacteristic ledTimingChar("20B10023-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 9);
BLEStringCharacteristic laserTimingChar("20B10024-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, 9);

BLEIntCharacteristic activeLasersChar("20B10025-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify);
BLEIntCharacteristic laserPowerChar("20B10026-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify);

BLEIntCharacteristic resetChar("20B10027-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify);

// Initial Device Values
int batteryLevel = 100;
int sessionDuration = 560;

char ledTiming[9] = "10101010";
char laserTiming[9] = "00001111";

int activeLasers = 100;
int laserPower = 100;

// Misc
int i = 0;
bool usbConnected = false;

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

  BLE.setDeviceName("MS_B_1234");
  BLE.setLocalName("MS_B_1234");

  BLE.setAdvertisedService(mitosportzService);

  mitosportzService.addCharacteristic(batteryLevelChar);
  mitosportzService.addCharacteristic(sessionDurationChar);
  mitosportzService.addCharacteristic(ledTimingChar);
  mitosportzService.addCharacteristic(laserTimingChar);
  mitosportzService.addCharacteristic(activeLasersChar);
  mitosportzService.addCharacteristic(laserPowerChar);
  mitosportzService.addCharacteristic(resetChar);

  BLE.addService(mitosportzService);

  sessionDurationChar.setEventHandler(BLEWritten, sessionDurationReceived);
  ledTimingChar.setEventHandler(BLEWritten, ledTimingReceived);
  laserTimingChar.setEventHandler(BLEWritten, laserTimingReceived);
  activeLasersChar.setEventHandler(BLEWritten, activeLasersReceived);
  laserPowerChar.setEventHandler(BLEWritten, laserPowerReceived);
  resetChar.setEventHandler(BLEWritten, resetReceived);

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

  BLE.poll();

  batteryLevelChar.setValue(batteryLevel);
  sessionDurationChar.setValue(sessionDuration);
  ledTimingChar.setValue(ledTiming);
  laserTimingChar.setValue(laserTiming);
  activeLasersChar.setValue(activeLasers);
  laserPowerChar.setValue(laserPower);

  Serial.print("Battery Level: " + String(batteryLevel) + "% ");
  Serial.print("Session Duration: " + String(sessionDuration) + "s ");
  Serial.print("LED Timing: " + String(ledTiming) + " ");
  Serial.print("Laser Timing: " + String(laserTiming) + " ");
  Serial.print("Active Lasers: " + String(activeLasers) + "% ");
  Serial.print("Laser Power: " + String(laserPower) + "% ");
  Serial.println(" ");

  delay(50);
  i++;
}

void sessionDurationReceived(BLEDevice central, BLECharacteristic characteristic) {
  Serial.println("Session Duration Received");
  sessionDuration = characteristic.value()[0];
}

void ledTimingReceived(BLEDevice central, BLECharacteristic characteristic) {
  Serial.println("LED Timing Received");
  for (int j = 0; j < characteristic.valueLength(); j++) {
    ledTiming[j] = characteristic.value()[j];
  }
}

void laserTimingReceived(BLEDevice central, BLECharacteristic characteristic) {
  Serial.println("Laser Timing Received");
  for (int j = 0; j < characteristic.valueLength(); j++) {
    int val = characteristic.value()[j];
    Serial.println(val);
    if (val == 48) {
      laserTiming[j] = '0';
    }
    if (val == 49) {
      laserTiming[j] = '1';
    }
  }
}

void activeLasersReceived(BLEDevice central, BLECharacteristic characteristic) {
  Serial.println("Active Lasers Received");
  activeLasers = characteristic.value()[0];
}

void laserPowerReceived(BLEDevice central, BLECharacteristic characteristic) {
  Serial.println("Laser Power Received");
  laserPower = characteristic.value()[0];
}

void resetReceived(BLEDevice central, BLECharacteristic characteristic) {
  Serial.println("RESET");
  for (int j=0; j < 10; j++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}
