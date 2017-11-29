/** @file */

/**
 * Originally written by Andri Yadi on 8/5/16
 * Maintained by Alwin Arrasyid
 */

#ifndef ALORA_ALORASENSORKIT_H
#define ALORA_ALORASENSORKIT_H

#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ClosedCube_HDC1080.h>
#include <NMEAGPS.h>
#include <Streamers.h>
#include "Adafruit_TSL2591.h"
#include <SparkFunCCS811.h>

#include "GpioExpander.h"

#include <RTClib.h>
#undef SECONDS_PER_DAY

#include "AllAboutEE_MAX11609.h"
using namespace AllAboutEE;

#include "AloraIMULSM9DS1Adapter.h"

/** Choose IMU sensor for Alora. Uses LSM9DS1 by default */
#ifndef ALORA_IMU_SENSOR
    #define ALORA_IMU_SENSOR AloraIMULSM9DS1Adapter
#endif

/** By default use CCS811 as the air quality sensor */
#ifndef ALORA_SENSOR_USE_CCS811
    #define ALORA_SENSOR_USE_CCS811 1
#endif

/** Define sensor query interval. The default value is 300ms */
#ifndef ALORA_SENSOR_QUERY_INTERVAL
    #define ALORA_SENSOR_QUERY_INTERVAL 300
#endif

/** Define Alora enable pin number. It is GPIO 16 on ESPectro32 board */
#ifndef ALORA_ENABLE_PIN
    #define ALORA_ENABLE_PIN 16
#endif

/** Enable MAX11609 by default. Set this definition value to 0 to disable MAX11069 */
#ifndef ALORA_USE_MAX11609
    #define ALORA_USE_MAX11609 1
#endif

/** Enable Air quality / gas sensor by default. Set this definition value to 0 to disable air quality / gas sensor. */
#ifndef ALORA_USE_AIR_QUALITY_GAS_SENSOR
    #define ALORA_USE_AIR_QUALITY_GAS_SENSOR 1
#endif

/** Enable IMU sensor by default. Set this definition value to 0 to disable IMU sensor */
#ifndef ALORA_USE_IMU_SENSOR
    #define ALORA_USE_IMU_SENSOR 1
#endif

/** Enable GPIO expander by default. Set this definition value to 0 to disable GPIO expander */
#ifndef ALORA_USE_GPIO_EXPANDER
    #define ALORA_USE_GPIO_EXPANDER 1
#endif

/** Enable HDC1080 by default. Set this definition value to 0 to disable HDC1080 */
#ifndef ALORA_USE_HDC1080_SENSOR
    #define ALORA_USE_HDC1080_SENSOR 1
#endif

/** Enable BME280 by default. Set this definition value to 0 to disable BME280 */
#ifndef ALORA_USE_BME280_SENSOR
    #define ALORA_USE_BME280_SENSOR 1
#endif

/** Enable TSL2591 by default. Set this definition value to 0 to disable TSL2591 */
#ifndef ALORA_USE_TSL2591_SENSOR
    #define ALORA_USE_TSL2591_SENSOR 1
#endif

/** HDC1080 I2C address */
#define ALORA_HDC1080_ADDRESS 0x40

/** CCS811 I2C address */
#define ALORA_I2C_ADDRESS_CCS811 0x5A

/** Magnetometer I2C address */
#if ALORA_IMU_SENSOR == ALORA_IMU_SENSOR_LSM9DS1
    #define ALORA_I2C_ADDRESS_IMU_M 0x1E
#elif ALORA_IMU_SENSOR == ALORA_IMU_SENSOR_LSM303AGR
    #define ALORA_I2C_ADDRESS_IMU_M 0x3C
#endif

/** Accelerometer and Gyroscope I2C address */
#if ALORA_IMU_SENSOR == ALORA_IMU_SENSOR_LSM9DS1
    #define ALORA_I2C_ADDRESS_IMU_AG 0x6B
#elif ALORA_IMU_SENSOR == ALORA_IMU_SENSOR_LSM303AGR
    #define ALORA_I2C_ADDRESS_IMU_AG 0x32
#endif

/** Magnetic sensor pin. It is GPIO 35 on ESPectro32 board */
#define ALORA_MAGNETIC_SENSOR_PIN 35

/** Heater pin for gas sensor. It is pin GPIO 13 on ESPectro32 board */
#define ALORA_ADC_GAS_HEATER_PIN 13

/** Channel number of MAX1109 for gas sensor */
#define ALORA_ADC_GAS_CHANNEL 1

/**
 * Data read from sensors are stored in this struct
 */
struct SensorValues {
    float T1;           /**< Temperature from BME280 in celcius unit */
    float P;            /**< Pressure from BME280 in hPa unit */
    float H1;           /**< Humidity from BME280 */
    float T2;           /**< Temperature from HDC1080 in celcius unit */
    float H2;           /**< Humidifty from HDC1080 */
    double lux;         /**< Luminance from TSL2591 */
    uint16_t gas;       /**< Air quality value from either CCS811 or analog gas sensor */
    uint16_t co2;       /**< CO2 readgin from CCS811 */
    float accelX;       /**< Accelerometer X axis */
    float accelY;       /**< Accelerometer Y axis */
    float accelZ;       /**< Accelerometer Z axis */
    float gyroX;        /**< Gyroscope X axis */
    float gyroY;        /**< Gyroscope Y axis */
    float gyroZ;        /**< Gyroscope Z axis */
    float magX;         /**< Magnometer X axis */
    float magY;         /**< Magnometer Y axis */
    float magZ;         /**< Magnometer Z axis */
    float magHeading;   /**< Heading in degrees */
    int magnetic;       /**< Magnetic sensor value */
    float windSpeed;    /**< Speed of the wind in MPH */
    gps_fix gpsFix;     /**< GPS fix information */
};


/**
 *  Alora Sensor Kit class.
 *  Main class for reading sensor on Alora board
 *  \example examples/AloraReadAllSensor/AloraReadAllSensor.ino
 *  \example examples/AloraReadGPS/AloraReadGPS.ino
 */
class AloraSensorKit {
public:
    AloraSensorKit();
    ~AloraSensorKit();

    void begin();
    void run();
    void scanAndPrintI2C(Print& print);
    void printSensingTo(Print& print);
    void printSensingTo(String& str);
    uint16_t readADC(uint8_t channel);
    DateTime getDateTime();
    SensorValues& getLastSensorData();
    void initGPS(Stream* gpsStream);
    NMEAGPS* getGPSObject();

private:
    NMEAGPS* gps = NULL;
    Stream* gpsStream = NULL;
    Adafruit_BME280* bme280 = NULL;                             /**< Object of Adafruit BME280 sensor */
    ClosedCube_HDC1080* hdc1080 = NULL;                         /**< Object of HDC1080 sensor */
    Adafruit_TSL2591* tsl2591 = NULL;                           /**< Object of Adafruit TSL2591 sensor */
    CCS811* ccs811 = NULL;                                      /**< Object of CCS811 sensor */
    ALORA_IMU_SENSOR* imuSensor = NULL;                         /**< IMU sensor adapter object */
    GpioExpander* ioExpander = NULL;                            /**< Object of GPIO Expander (SX1509) */
    MAX11609* max11609 = NULL;                                  /**< Object of MAX11609 */
    RTC_DS3231* rtc = NULL;                                     /**< Object of RTC sensor */

    SensorValues lastSensorData;                                /**< Object of SensorValues struct. All sensor data are stored in this property */
    uint32_t lastSensorQuerryMs = 0;                            /**< Records the time when the sensor data is read in milliseconds */

    void doAllSensing();
    void readBME280(float& T, float& P, float& H);
    void readHDC1080(float& T, float& H);
    void readTSL2591(double& lux);
    void configureTSL2591Sensor();
    void readGas(uint16_t& gas, uint16_t& co2);
    void readAccelerometer(float &ax, float &ay, float &az);
    void readMagnetometer(float &mx, float &my, float &mz, float &mH);
    void readGyro(float &gx, float &gy, float &gz);
    void readMagneticSensor(int& mag);
    void readWindSpeed(float& windspeed);
    void readGPS(gps_fix& fix);
};

#endif
