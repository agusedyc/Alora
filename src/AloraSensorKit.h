/*
 * Originally written by Andri Yadi on 8/5/16
 * Maintained by Alwin Arrasyid
 */

#ifndef ALORA_ALORASENSORKIT_H
#define ALORA_ALORASENSORKIT_H

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#ifndef ALORA_SENSOR_QUERY_INTERVAL
#define ALORA_SENSOR_QUERY_INTERVAL 4000
#endif

struct SensorValues {
    float T1;
    float P;
    float H1;
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
    SensorValues lastSensorData;
    uint32_t lastSensorQuerryMs = 0;

    void doAllSensing();
    void readBME280(float& T, float& P, float& H);
};

#endif
