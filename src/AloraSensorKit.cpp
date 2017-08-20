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
            Serial.println("[ERROR] Failed to init BME280");
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
            Serial.println("[ERROR] CCS811 .begin() returned with an error.");
            Serial.printf("[ERROR] CCS811 Init return code %d\n",  returnCode);
    
            delete ccs811;
            ccs811 = NULL;
        } else {
            Serial.printf("[DEBUG] CCS811 Init return code %d\n",  returnCode);
        }
    }

    if (imuSensor == NULL) {
        imuSensor = new LSM9DS1();
        imuSensor->settings.device.commInterface =IMU_MODE_I2C;
        imuSensor->settings.device.mAddress = ALORA_I2C_ADDRESS_IMU_M;
        imuSensor->settings.device.agAddress = ALORA_I2C_ADDRESS_IMU_AG;

        if (!imuSensor->begin()) {
            Serial.println("[ERROR] Failed initializing IMU sensor");
            delete imuSensor;
            imuSensor = NULL;
        }
    }

    if (ioExpander == NULL) {
        ioExpander = new GpioExpander();
        if (ioExpander->begin()) {
            ioExpander->pinMode(4, OUTPUT);
            ioExpander->digitalWrite(4, HIGH);

            // IMU enable
            ioExpander->pinMode(7, OUTPUT);
            ioExpander->digitalWrite(7, HIGH);

            // GPS enable
            ioExpander->pinMode(12, OUTPUT);
            ioExpander->digitalWrite(12, HIGH);

            ioExpander->pinMode(6, OUTPUT);
            ioExpander->digitalWrite(6, HIGH);
        } else {
            Serial.println("[ERROR] Failed to initialize SX1509 IO Expander");
            delete ioExpander;
            ioExpander = NULL;
        }
    }

    pinMode(ALORA_MAGNETIC_SENSOR_PIN, INPUT);
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
    sprintf(bme280PayloadStr, "[BME280] T = %s *C\tP = %s Pa\tH = %s\r\n", tStr, pStr, hStr);

    dtostrf(lastSensorData.T2, 6, 2, tStr);
    dtostrf(lastSensorData.H2, 6, 2, hStr);
    char hdcPayloadStr[40];
    sprintf(hdcPayloadStr, "[HDC1080] T = %s *C\tH = %s\r\n", tStr, hStr);

    char gasPayloadStr[40];
    sprintf(gasPayloadStr, "[GAS & CO2] Gas = %d\tCO2 = %d\r\n", lastSensorData.gas, lastSensorData.co2);

    char luxStr[15];
    dtostrf((float)lastSensorData.lux, 10, 4, luxStr);
    char lightPayloadStr[40];
    sprintf(lightPayloadStr, "[Light Sensor] %s Lux\r\n", luxStr);

    char xStr[9], yStr[9], zStr[9];
    dtostrf(lastSensorData.accelX, 6, 2, xStr);
    dtostrf(lastSensorData.accelY, 6, 2, yStr);
    dtostrf(lastSensorData.accelZ, 6, 2, zStr);
    char accelPayloadStr[64];
    sprintf(accelPayloadStr, "[ACCEL] X = %s\tY = %s\tZ = %s\r\n", xStr, yStr, zStr);

    dtostrf(lastSensorData.gyroX, 6, 2, xStr);
    dtostrf(lastSensorData.gyroY, 6, 2, yStr);
    dtostrf(lastSensorData.gyroZ, 6, 2, zStr);
    char gyroPayloadStr[64];
    sprintf(gyroPayloadStr, "[GYRO] X = %s\tY = %s\tZ = %s\r\n", xStr, yStr, zStr);

    char magHeadingStr[9];
    dtostrf(lastSensorData.magX, 6, 2, xStr);
    dtostrf(lastSensorData.magY, 6, 2, yStr);
    dtostrf(lastSensorData.magZ, 6, 2, zStr);
    dtostrf(lastSensorData.magHeading, 6, 2, magHeadingStr);
    char magPayloadStr[64];
    sprintf(magPayloadStr, "[MAG] X = %s\tY = %s\tZ = %s\tHd = %s Deg\r\n", xStr, yStr, zStr, magHeadingStr);

    char magnetic[2];
    sprintf(magnetic, "%d", lastSensorData.magnetic);
    char magneticPayloadStr[40];
    sprintf(magneticPayloadStr, "[Magnetic] %s \r\n", magnetic);

    str = String(bme280PayloadStr) + String(hdcPayloadStr) + String(gasPayloadStr);
    str += String(accelPayloadStr) + String(gyroPayloadStr) + String(magPayloadStr);
    str += String(lightPayloadStr) + String(magneticPayloadStr);
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

void AloraSensorKit::readMagneticSensor(int& mag) {
    mag = digitalRead(ALORA_MAGNETIC_SENSOR_PIN);
}

void AloraSensorKit::readAccelerometer(float &ax, float &ay, float &az) {
    if (imuSensor == NULL) {
        ax = 0.0;
        ay = 0.0;
        az = 0.0;

        return;
    }
    imuSensor->readAccel();

    ax = imuSensor->calcAccel(imuSensor->ax);
    ay = imuSensor->calcAccel(imuSensor->ay);
    az = imuSensor->calcAccel(imuSensor->az);
}

void AloraSensorKit::readGyro(float &gx, float &gy, float &gz) {
    if (imuSensor == NULL) {
        gx = 0.0;
        gy = 0.0;
        gz = 0.0;
    }

    imuSensor->readGyro();

    gx = imuSensor->calcGyro(imuSensor->gx);
    gy = imuSensor->calcGyro(imuSensor->gy);
    gz = imuSensor->calcGyro(imuSensor->gz);
}

void AloraSensorKit::readMagnetometer(float &mx, float &my, float &mz, float &mH) {
    if (imuSensor == NULL) {
        mx = 0.0;
        my = 0.0;
        mz = 0.0;
        mH = 0.0;

        return;
    }

    imuSensor->readMag();

    mx = imuSensor->calcMag(imuSensor->mx);
    my = imuSensor->calcMag(imuSensor->my);
    mz = imuSensor->calcMag(imuSensor->mz);

    float heading;

    if (imuSensor->my > 0)
    {
        heading = 90 - (atan(mx / my) * (180 / PI));
    }
    else if (imuSensor->my < 0)
    {
        heading = -(atan(mx / my) * (180 / PI));
    }
    else // hy = 0
    {
        if (mx < 0)
            heading = 180;
        else
            heading = 0;
    }

    mH = heading;
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

    float T1, P, H1;
    readBME280(T1, P, H1);

    lastSensorData.T1 = T1;
    lastSensorData.P = P;
    lastSensorData.H1 = H1;

    float T2, H2;
    readHDC1080(T2, H2);
    lastSensorData.T2 = T2;
    lastSensorData.H2 = H2;

    double lux;
    readTSL2591(lux);
    lastSensorData.lux = lux;

    uint16_t gas, co2;
    readGas(gas, co2);
    lastSensorData.gas = gas;
    lastSensorData.co2 = co2;

    float X, Y, Z;
    readAccelerometer(X, Y, Z);
    lastSensorData.accelX = X;
    lastSensorData.accelY = Y;
    lastSensorData.accelZ = Z;

    float gX, gY, gZ;
    readGyro(gX, gY, gZ);
    lastSensorData.gyroX = gX;
    lastSensorData.gyroY = gY;
    lastSensorData.gyroZ = gZ;

    float mX, mY, mZ, mH;
    readMagnetometer(mX, mY, mZ, mH);
    lastSensorData.magX = mX;
    lastSensorData.magY = mY;
    lastSensorData.magZ = mZ;
    lastSensorData.magHeading = mH;

    int mag;
    readMagneticSensor(mag);
    lastSensorData.magnetic = mag;
}
