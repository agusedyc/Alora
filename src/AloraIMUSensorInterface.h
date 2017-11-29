/** @file */

#ifndef ALORA_IMU_SENSOR_INTERFACE_H
#define ALORA_IMU_SENSOR_INTERFACE_H

#include <stdint.h>

/**
 * @brief Abstract class for IMU sensor adapter on Alora board.
 *
 */
class AloraIMUSensorBase {
public:
    AloraIMUSensorBase() {}
    virtual ~AloraIMUSensorBase() {}

    /**
     * @brief
     *
     * @param accAddress accelerometer and gyroscope (if any) I2C address
     * @param magAddress magnetometer I2C address
     * @return true if IMU sensor is initialized successfully
     * @return false if IMU sensor is not initialized
     */
    virtual bool begin(uint8_t accAddress, uint8_t magAddress) = 0;

    /**
     * @brief Read X axis value from accelerometer
     *
     * @return float X axis value
     */
    virtual float readAccelX() = 0;

    /**
     * @brief Read Y axis value from accelerometer
     *
     * @return float Y axis value
     */
    virtual float readAccelY() = 0;

    /**
     * @brief Read Z axis value from accelerometer
     *
     * @return float Z axis value
     */
    virtual float readAccelZ() = 0;

    /**
     * @brief Read X axis value from gyroscope
     *
     * @return float X axis value
     */
    virtual float readGyroX() = 0;

    /**
     * @brief Read Y axis value from gyroscope
     *
     * @return float Y axis value
     */
    virtual float readGyroY() = 0;

    /**
     * @brief Read Z axis value from gyroscope
     *
     * @return float Z axis value
     */
    virtual float readGyroZ() = 0;

    /**
     * @brief Read X axis value from magnetometer
     *
     * @return float X axis value
     */
    virtual float readMagX() = 0;

    /**
     * @brief Read Y axis value from magnetometer
     *
     * @return float Y axis value
     */
    virtual float readMagY() = 0;

    /**
     * @brief Read Z axis value from magnetometer
     *
     * @return float Z axis value
     */
    virtual float readMagZ() = 0;

    /**
     * @brief Read heading value from magnetometer
     *
     * @return float heading in degree unit
     */
    virtual float readMagHeading() = 0;
};

#endif
