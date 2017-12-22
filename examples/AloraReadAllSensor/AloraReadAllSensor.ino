#include <Arduino.h>
#include <Wire.h>
#include <AloraSensorKit.h>


// Use 13 for Alora board v2.2
#define ENABLE_PIN 16

// Use LOW for Alora board v2.2
#define ENABLE_PIN_ACTIVE_LOGIC HIGH

AloraSensorKit sensorKit(ENABLE_PIN, ENABLE_PIN_ACTIVE_LOGIC);

void setup() {
    // NOTE: in case the process hang right after initializing alora library, uncomment this line below:
    // Wire.begin();

    Serial.begin(9600);
    sensorKit.begin();
}

void loop() {
    // read all sensors
    sensorKit.run();

    // get sensor values
    SensorValues sensorData = sensorKit.getLastSensorData();

    // sensorData.T1 and sensorData.T2 are temperature sensor data
    Serial.print("Temperature data:");
    Serial.println(sensorData.T1);

    // sensorData.H1 and sensorData.H2 are humidity sensor data
    Serial.println("Humidity data:");
    Serial.println(sensorData.H1);

    delay(1000);
}
