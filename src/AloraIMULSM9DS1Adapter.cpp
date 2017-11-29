#include "AloraIMULSM9DS1Adapter.h"

AloraIMULSM9DS1Adapter::AloraIMULSM9DS1Adapter() {
}

AloraIMULSM9DS1Adapter::~AloraIMULSM9DS1Adapter() {
    delete imuSensor;
}

bool AloraIMULSM9DS1Adapter::begin(uint8_t accAddress, uint8_t magAddress) {
    imuSensor = new LSM9DS1();
    imuSensor->settings.device.commInterface = IMU_MODE_I2C;
    imuSensor->settings.device.mAddress = magAddress;
    imuSensor->settings.device.agAddress = accAddress;

    return imuSensor->begin();
}

float AloraIMULSM9DS1Adapter::readAccelX() {
    imuSensor->readAccel();

    return imuSensor->calcAccel(imuSensor->ax);
}

float AloraIMULSM9DS1Adapter::readAccelY() {
    imuSensor->readAccel();

    return imuSensor->calcAccel(imuSensor->ay);
}

float AloraIMULSM9DS1Adapter::readAccelZ() {
    imuSensor->readAccel();

    return imuSensor->calcAccel(imuSensor->az);
}

float AloraIMULSM9DS1Adapter::readGyroX() {
    imuSensor->readGyro();

    return imuSensor->calcGyro(imuSensor->gx);
}

float AloraIMULSM9DS1Adapter::readGyroY() {
    imuSensor->readGyro();

    return imuSensor->calcGyro(imuSensor->gy);
}

float AloraIMULSM9DS1Adapter::readGyroZ() {
    imuSensor->readGyro();

    return imuSensor->calcGyro(imuSensor->gz);
}

float AloraIMULSM9DS1Adapter::readMagX() {
    imuSensor->readMag();

    return imuSensor->calcMag(imuSensor->mx);
}

float AloraIMULSM9DS1Adapter::readMagY() {
    imuSensor->readMag();

    return imuSensor->calcMag(imuSensor->my);
}

float AloraIMULSM9DS1Adapter::readMagZ() {
    imuSensor->readMag();

    return imuSensor->calcMag(imuSensor->mz);
}

float AloraIMULSM9DS1Adapter::readMagHeading() {
    float heading = 0.0;
    float mx = readMagX();
    float my = readMagY();
    float mz = readMagZ();

    if (imuSensor->my > 0) {
        heading = 90 - (atan(mx / my) * (180 / PI));
    } else if (imuSensor->my < 0) {
        heading = -(atan(mx / my) * (180 / PI));
    } else {
        if (mx < 0) {
            heading = 180.0;
        } else {
            heading = 0.0;
        }
    }

    return heading;
}
