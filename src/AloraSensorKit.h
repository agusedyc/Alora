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

#ifndef ALORA_SENSOR_QUERY_INTERVAL
#define ALORA_SENSOR_QUERY_INTERVAL 4000
#endif

#define ALORA_HDC1080_ADDRESS 0x40

struct SensorValues {
    float T1;
    float P;
    float H1;
    float T2;
    float H2;
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

    SensorValues lastSensorData;
    uint32_t lastSensorQuerryMs = 0;

    void doAllSensing();
    void readBME280(float& T, float& P, float& H);
    void readHDC1080(float& T, float& H);
};

#endif
