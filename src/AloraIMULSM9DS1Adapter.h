/** @file */

#ifndef ALORA_IMU_LSM9DS1_ADAPTER_H
#define ALORA_IMU_LSM9DS1_ADAPTER_H

#include <Arduino.h>
#include "SparkFunLSM9DS1.h"
#include "AloraIMUSensorInterface.h"

class AloraIMULSM9DS1Adapter: public AloraIMUSensorBase {
public:
    AloraIMULSM9DS1Adapter();
    virtual ~AloraIMULSM9DS1Adapter();

    virtual bool begin(uint8_t accAddress, uint8_t magAddress);
    virtual float readAccelX();
    virtual float readAccelY();
    virtual float readAccelZ();

    virtual float readGyroX();
    virtual float readGyroY();
    virtual float readGyroZ();

    virtual float readMagX();
    virtual float readMagY();
    virtual float readMagZ();
    virtual float readMagHeading();

private:
    LSM9DS1* imuSensor;                     /**< LSM9DS1 object pointer */
};

#endif
