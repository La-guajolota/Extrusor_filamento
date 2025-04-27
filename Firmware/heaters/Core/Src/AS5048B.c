/**
 * AS5048B Magnetic Position Sensor Driver Implementation
 *
 * This file contains the implementation of the driver functions
 * for the AS5048B magnetic position sensor using I2C communication.
 */

#include "AS5048B.h"

/**
 * Read data from sensor via I2C
 *
 * @param dev_id Device I2C address
 * @param reg_addr Register address to read from
 * @param reg_data Pointer to store read data
 * @param len Number of bytes to read
 * @return 0 for success, non-zero for failure
 */
static int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
	HAL_StatusTypeDef rslt = HAL_OK; /* Return 0 for Success, non-zero for failure */

    /*
     * I2C Read Transaction:
     * 1. Start
     * 2. Write register address
     * 3. Restart
     * 4. Read data bytes with ACK except last byte with NAK
     * 5. Stop
     */

    /* 1.- Send start condition and write address */
    /* 2.- Send register address */
    rslt = HAL_I2C_Master_Transmit(&hi2c1, dev_id, &reg_addr, 1, HAL_MAX_DELAY);

    /* 3.- Send restart for read operation */
    /* 4.- Read all bytes except the last one with ACK */
    /* 5.- Read last byte with NAK */
    /* 6.- Send stop condition */
    rslt =  HAL_I2C_Master_Receive(&hi2c1, dev_id, reg_data, len, HAL_MAX_DELAY);

    return rslt;
}

/**
 * Write data to sensor via I2C
 *
 * @param dev_id Device I2C address
 * @param reg_addr Register address to write to
 * @param reg_data Pointer to data to write
 * @param len Number of bytes to write
 * @return 0 for success, non-zero for failure
 */
static int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
	HAL_StatusTypeDef rslt = HAL_OK; /* Return 0 for Success, non-zero for failure */
    /*
     * I2C Write Transaction:
     * 1. Start
     * 2. Write register address
     * 3. Write data bytes
     * 4. Stop
     */

    /* 1.- Send start condition and write address */
    /* 2.- Send register address */
    rslt = HAL_I2C_Master_Transmit(&hi2c1, dev_id, &reg_addr, 1, HAL_MAX_DELAY);

    /* 3.- Write all data bytes */
    /* 4.- Stop */
    rslt = HAL_I2C_Master_Transmit(&hi2c1, dev_id, &reg_data, len, HAL_MAX_DELAY);

    return rslt;
}

/**
 * Updates all register values from the sensor
 *
 * @param sensor Pointer to sensor structure
 * @return Status code (0 for success)
 */
int AS5048B_UpdateRegisters(AS5048B_Sensor *sensor)
{
    uint8_t reg_data[10];
    int8_t rslt = 0;

    /* Read Programming Control register */
    rslt = user_i2c_read(AS5048B_I2C_ADDRESS0, REG_PROG_CTRL, reg_data, 1);

    /* Read I2C Address and Zero Position registers */
    rslt = user_i2c_read(AS5048B_I2C_ADDRESS0, REG_I2C_ADDR, reg_data + 1, 2);

    /* Read AGC through Angle registers */
    rslt = user_i2c_read(AS5048B_I2C_ADDRESS0, REG_AGC, reg_data + 4, 5);

    /* Update structure with read values */
    sensor->registers.programming_control = reg_data[0];
    sensor->registers.i2c_slave_address = reg_data[1];
    sensor->registers.zero_position_high = reg_data[2];
    sensor->registers.zero_position_low = reg_data[3];
    sensor->registers.automatic_gain_control = reg_data[4];
    sensor->registers.diagnostics = reg_data[5];
    sensor->registers.magnitude_high = reg_data[6];
    sensor->registers.magnitude_low = reg_data[7];
    sensor->registers.angle_high = reg_data[8];
    sensor->registers.angle_low = reg_data[9] >> 2; /* Filter out 2 LSBs to improve stability */

    return rslt;
}

/**
 * Sets the mechanical zero position of the sensor
 *
 * @param sensor Pointer to sensor structure
 * @return Status code (0 for success)
 */
int AS5048B_SetZeroPosition(AS5048B_Sensor *sensor)
{
    uint8_t reg_data[2] = {0, 0};
    int8_t rslt = 0;

    /* First clear the zero position */
    rslt = user_i2c_write(AS5048B_I2C_ADDRESS0, REG_ZERO_POS_HIGH, reg_data, 1);

    /* Update registers to get current angle */
    AS5048B_UpdateRegisters(sensor);

    /* Set the zero position to current angle */
    reg_data[0] = sensor->registers.angle_high;
    reg_data[1] = sensor->registers.angle_low;
    rslt = user_i2c_write(AS5048B_I2C_ADDRESS0, REG_ZERO_POS_HIGH, reg_data, 2);

    return rslt;
}

/**
 * Gets the current angle in degrees (0-360)
 *
 * @param sensor Pointer to sensor structure
 * @return Angle in degrees as float
 */
float AS5048B_GetAngleDegrees(AS5048B_Sensor *sensor)
{
    /* Update registers to get current angle */
    AS5048B_UpdateRegisters(sensor);

    /* Calculate angle in degrees */
    uint16_t raw_angle = (sensor->registers.angle_high << 6) | sensor->registers.angle_low;
    float angle_degrees = (float)raw_angle * 360.0f / 16384.0f; /* 14-bit resolution (2^14 = 16384) */

    return angle_degrees;
}

/**
 * Gets the current angle in radians (0-2π)
 *
 * @param sensor Pointer to sensor structure
 * @return Angle in radians as float
 */
float AS5048B_GetAngleRadians(AS5048B_Sensor *sensor)
{
    float angle_degrees = AS5048B_GetAngleDegrees(sensor);
    float angle_radians = angle_degrees * 3.14159265359f / 180.0f;

    return angle_radians;
}

/**
 * Gets the current magnitude (signal strength)
 *
 * @param sensor Pointer to sensor structure
 * @return Magnitude value (0-4095)
 */
uint16_t AS5048B_GetMagnitude(AS5048B_Sensor *sensor)
{
    /* Update registers to get current magnitude */
    AS5048B_UpdateRegisters(sensor);

    /* Calculate magnitude */
    uint16_t magnitude = (sensor->registers.magnitude_high << 6) | sensor->registers.magnitude_low;

    return magnitude;
}

/**
 * Checks diagnostic information
 *
 * @param sensor Pointer to sensor structure
 * @return 0 if no errors, error code otherwise
 */
uint8_t AS5048B_CheckDiagnostics(AS5048B_Sensor *sensor)
{
    /* Update registers to get current diagnostics */
    AS5048B_UpdateRegisters(sensor);

    return sensor->registers.diagnostics;
}

