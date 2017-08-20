/*
 * Originally written by Andri Yadi on 8/5/16
 * Maintained by Alwin Arrasyid
 */

#ifndef ALORA_ALORASENSORKIT_H
#define ALORA_ALORASENSORKIT_H

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ClosedCube_HDC1080.h>
#include <Adafruit_TSL2591.h>
#include <SparkFunCCS811.h>
#include <SparkFunLSM9DS1.h>
#include "GpioExpander.h"

#ifndef ALORA_SENSOR_QUERY_INTERVAL
#define ALORA_SENSOR_QUERY_INTERVAL 300
#endif

#define ALORA_HDC1080_ADDRESS 0x40
#define ALORA_I2C_ADDRESS_CCS811 0x5A
#define ALORA_I2C_ADDRESS_IMU_M 0x1E
#define ALORA_I2C_ADDRESS_IMU_AG 0x6B

struct SensorValues {
    float T1;
    float P;
    float H1;
    float T2;
    float H2;
    double lux;
    uint16_t gas;
    uint16_t co2;
    float accelX;
    float accelY;
    float accelZ;
    float gyroX;
    float gyroY;
    float gyroZ;
    float magX;
    float magY;
    float magZ;
    float magHeading;
};

class AloraSensorKit {
public:
    AloraSensorKit();
    ~AloraSensorKit();

    void begin();
    void run();
    void scanAndPrintI2C(Print& print);
    void printSensingTo(Print& print);
    void printSensingTo(String& str);
private:
    Adafruit_BME280* bme280 = NULL;
    ClosedCube_HDC1080* hdc1080 = NULL;
    Adafruit_TSL2591* tsl2591 = NULL;
    CCS811* ccs811 = NULL;
    LSM9DS1* imuSensor = NULL;
    GpioExpander* ioExpander = NULL;

    SensorValues lastSensorData;
    uint32_t lastSensorQuerryMs = 0;

    void doAllSensing();
    void readBME280(float& T, float& P, float& H);
    void readHDC1080(float& T, float& H);
    void readTSL2591(double& lux);
    void configureTSL2591Sensor();
    void readGas(uint16_t& gas, uint16_t& co2);
    void readAccelerometer(float &ax, float &ay, float &az);
    void readMagnetometer(float &mx, float &my, float &mz, float &mH);
    void readGyro(float &ax, float &ay, float &az);
};

#endif
