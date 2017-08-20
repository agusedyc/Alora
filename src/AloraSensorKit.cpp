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

    if (hdc1080 == NULL) {
        hdc1080 = new ClosedCube_HDC1080();
        hdc1080->begin(ALORA_HDC1080_ADDRESS);
    }

    if (tsl2591 == NULL) {
        tsl2591 = new Adafruit_TSL2591(2591);
        configureTSL2591Sensor();
    }

    if (ccs811 == NULL) {
        ccs811 = new CCS811(ALORA_I2C_ADDRESS_CCS811);

        CCS811Core::status returnCode = ccs811->begin();
        if (returnCode != CCS811Core::SENSOR_SUCCESS) {
            Serial.println("[CCS811] .begin() returned with an error.");
            Serial.printf("[CCS811] Init return code %d\n",  returnCode);
    
            delete ccs811;
            ccs811 = NULL;
        } else {
            Serial.printf("[CCS811] Init return code %d\n",  returnCode);
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

    dtostrf(lastSensorData.T2, 6, 2, tStr);
    dtostrf(lastSensorData.H2, 6, 2, hStr);
    char hdcPayloadStr[40];
    sprintf(hdcPayloadStr, "[HDC1080] T = %s *C, H = %s\r\n", tStr, hStr);

    char gasPayloadStr[40];
    sprintf(gasPayloadStr, "[GAS & CO2] Gas = %d, CO2 = %d\r\n", lastSensorData.gas, lastSensorData.co2);

    char luxStr[15];
    dtostrf((float)lastSensorData.lux, 10, 4, luxStr);
    char lightPayloadStr[40];
    sprintf(lightPayloadStr, "[Light Sensor] %s Lux\r\n", luxStr);

    str = String(bme280PayloadStr) + String(hdcPayloadStr) + String(lightPayloadStr) + String(gasPayloadStr);
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

void AloraSensorKit::readHDC1080(float& T, float& H) {
    if (hdc1080 == NULL) {
        T = 0.0;
        H = 0.0;

        return;
    }

    T = hdc1080->readTemperature();
    H = hdc1080->readHumidity();
}

void AloraSensorKit::readTSL2591(double &lux) {
    uint16_t x = tsl2591->getLuminosity(TSL2591_VISIBLE);
    lux = (double)x;
}

void AloraSensorKit::configureTSL2591Sensor() {
    // You can change the gain on the fly, to adapt to brighter/dimmer light situations
    //tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
    tsl2591->setGain(TSL2591_GAIN_MED);      // 25x gain
    // tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain

    // Changing the integration time gives you a longer time over which to sense light
    // longer timelines are slower, but are good in very low light situtations!
    tsl2591->setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
    // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
    // tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
    // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
    // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
    // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

    /* Display the gain and integration time for reference sake */  
    Serial.println(F("------------------------------------"));
    Serial.print  (F("Gain:         "));
    tsl2591Gain_t gain = tsl2591->getGain();
    switch(gain)
    {
        case TSL2591_GAIN_LOW:
            Serial.println(F("1x (Low)"));
            break;
        case TSL2591_GAIN_MED:
            Serial.println(F("25x (Medium)"));
            break;
        case TSL2591_GAIN_HIGH:
            Serial.println(F("428x (High)"));
            break;
        case TSL2591_GAIN_MAX:
            Serial.println(F("9876x (Max)"));
            break;
    }
    Serial.print  (F("Timing:       "));
    Serial.print((tsl2591->getTiming() + 1) * 100, DEC); 
    Serial.println(F(" ms"));
    Serial.println(F("------------------------------------"));
    Serial.println(F(""));
}

void AloraSensorKit::readGas(uint16_t& gas, uint16_t& co2) {
    if (ccs811 == NULL) {
        gas = 0;
        co2 = 0;
    }

    if (!ccs811->dataAvailable()) { 
        return;
    }

    ccs811->readAlgorithmResults();
    uint16_t airTvoc = ccs811->getTVOC();
    uint16_t co2val = ccs811->getCO2();

    // Serial.print("[CCS811] CO2: ");
    // //Returns calculated CO2 reading
    // Serial.print(co2val);
    // Serial.print(", tVOC: ");
    // //Returns calculated TVOC reading
    // Serial.print(airTvoc);
    // Serial.println("");

    gas = airTvoc;
    co2 = co2val;
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

    readHDC1080(T, P);
    lastSensorData.T2 = T;
    lastSensorData.H2 = H;

    double lux;
    readTSL2591(lux);
    lastSensorData.lux = lux;

    uint16_t gas, co2;
    readGas(gas, co2);
    lastSensorData.gas = gas;
    lastSensorData.co2 = co2;
}
