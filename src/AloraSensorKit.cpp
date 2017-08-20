#include "AloraSensorKit.h"

AloraSensorKit::AloraSensorKit() {
}

AloraSensorKit::~AloraSensorKit() {
}

void AloraSensorKit::begin() {
    pinMode(16, OUTPUT);
    digitalWrite(16, HIGH);

    if (bme280 == NULL) {
        bme280 = new Adafruit_BME280();

        if (!bme280->begin()) {
            Serial.println("Failed to init BME280");
            delete bme280;
            bme280 = NULL;
        }
    }
}

void AloraSensorKit::run() {
    doAllSensing();
}

void AloraSensorKit::printSensingTo(Print& print) {
    print.println("Sensing:");
    
    String senseStr;
    printSensingTo(senseStr);
    print.println(senseStr);
}

void AloraSensorKit::printSensingTo(String& str) {
    doAllSensing();

    // BME280
    char tStr[9], pStr[9], hStr[9];
    dtostrf(lastSensorData.T1, 6, 2, tStr);
    dtostrf(lastSensorData.P, 6, 2, pStr);
    dtostrf(lastSensorData.H1, 6, 2, hStr);
    char bme280PayloadStr[64];
    sprintf(bme280PayloadStr, "[BME280] T = %s *C, P = %s Pa, H = %s\r\n", tStr, pStr, hStr);

    str = String(bme280PayloadStr);
}

void AloraSensorKit::scanAndPrintI2C(Print& print) {
    byte error;
    byte address;

    print.println("I2C scanning process is started");

    int foundDevices = 0;
    for (address = 0; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            print.print("Found I2C device at ");
            if (address < 16) {
                print.print("0");
            }

            print.print(address, HEX);
            print.println(" !");

            foundDevices++;
        }
    }

    if (foundDevices == 0) {
        print.println("No I2C devices found\n");
    } else {
        print.println("DONE\n");
    }
}

void AloraSensorKit::readBME280(float& T, float& P, float& H) {
    if (bme280 == NULL) {
        T = 0.0;
        P = 0.0;
        H = 0.0;

        return;
    }

    T = bme280->readTemperature();
    P = bme280->readPressure();
    H = bme280->readHumidity();
}

void AloraSensorKit::doAllSensing() {
    if (millis() - lastSensorQuerryMs < ALORA_SENSOR_QUERY_INTERVAL) {
        return;
    }

    lastSensorQuerryMs = millis();

    float T, P, H;
    readBME280(T, P, H);

    lastSensorData.T1 = T;
    lastSensorData.P = P;
    lastSensorData.H1 = H;
}
