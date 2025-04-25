///**
// * AS5048B Magnetic Position Sensor Driver Implementation
// *
// * This file contains the implementation of the driver functions
// * for the AS5048B magnetic position sensor using I2C communication.
// */
//
//#include "AS5048B.h"
//
///**
// * Read data from sensor via I2C
// *
// * @param dev_id Device I2C address
// * @param reg_addr Register address to read from
// * @param reg_data Pointer to store read data
// * @param len Number of bytes to read
// * @return 0 for success, non-zero for failure
// */
//static int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
//{
//    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */
//    uint16_t i = 0;
//
//    /*
//     * I2C Read Transaction:
//     * 1. Start
//     * 2. Write register address
//     * 3. Restart
//     * 4. Read data bytes with ACK except last byte with NAK
//     * 5. Stop
//     */
//
//    /* Send start condition and write address */
//    rslt = I2C_MasterSendStart(dev_id, I2C_WRITE_XFER_MODE);
//
//    /* Send register address */
//    rslt = I2C_MasterWriteByte(reg_addr);
//
//    /* Send restart for read operation */
//    rslt = I2C_MasterSendRestart(dev_id, I2C_READ_XFER_MODE);
//
//    /* Read all bytes except the last one with ACK */
//    while (i < (len - 1)) {
//        reg_data[i] = I2C_MasterReadByte(I2C_ACK_DATA);
//        i++;
//    }
//
//    /* Read last byte with NAK */
//    reg_data[i] = I2C_MasterReadByte(I2C_NAK_DATA);
//
//    /* Send stop condition */
//    rslt = I2C_MasterSendStop();
//
//    return rslt;
//}
//
///**
// * Write data to sensor via I2C
// *
// * @param dev_id Device I2C address
// * @param reg_addr Register address to write to
// * @param reg_data Pointer to data to write
// * @param len Number of bytes to write
// * @return 0 for success, non-zero for failure
// */
//static int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
//{
//    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */
//    uint16_t i = 0;
//
//    /*
//     * I2C Write Transaction:
//     * 1. Start
//     * 2. Write register address
//     * 3. Write data bytes
//     * 4. Stop
//     */
//
//    /* Send start condition and write address */
//    rslt = I2C_MasterSendStart(dev_id, I2C_WRITE_XFER_MODE);
//
//    /* Send register address */
//    rslt = I2C_MasterWriteByte(reg_addr);
//
//    /* Write all data bytes */
//    while (i < len) {
//        rslt = I2C_MasterWriteByte(*reg_data);
//        i++;
//        reg_data++;
//    }
//
//    /* Send stop condition */
//    rslt = I2C_MasterSendStop();
//
//    return rslt;
//}
//
///**
// * Updates all register values from the sensor
// *
// * @param sensor Pointer to sensor structure
// * @return Status code (0 for success)
// */
//int AS5048B_UpdateRegisters(AS5048B_Sensor *sensor)
//{
//    uint8_t reg_data[10];
//    int8_t rslt = 0;
//
//    /* Read Programming Control register */
//    rslt = user_i2c_read(AS5048B_PHYSICAL_ADDR, REG_PROG_CTRL, reg_data, 1);
//
//    /* Read I2C Address and Zero Position registers */
//    rslt = user_i2c_read(AS5048B_PHYSICAL_ADDR, REG_I2C_ADDR, reg_data + 1, 2);
//
//    /* Read AGC through Angle registers */
//    rslt = user_i2c_read(AS5048B_PHYSICAL_ADDR, REG_AGC, reg_data + 4, 5);
//
//    /* Update structure with read values */
//    sensor->registers.programming_control = reg_data[0];
//    sensor->registers.i2c_slave_address = reg_data[1];
//    sensor->registers.zero_position_high = reg_data[2];
//    sensor->registers.zero_position_low = reg_data[3];
//    sensor->registers.automatic_gain_control = reg_data[4];
//    sensor->registers.diagnostics = reg_data[5];
//    sensor->registers.magnitude_high = reg_data[6];
//    sensor->registers.magnitude_low = reg_data[7];
//    sensor->registers.angle_high = reg_data[8];
//    sensor->registers.angle_low = reg_data[9] >> 2; /* Filter out 2 LSBs to improve stability */
//
//    return rslt;
//}
//
///**
// * Sets the mechanical zero position of the sensor
// *
// * @param sensor Pointer to sensor structure
// * @return Status code (0 for success)
// */
//int AS5048B_SetZeroPosition(AS5048B_Sensor *sensor)
//{
//    uint8_t reg_data[2] = {0, 0};
//    int8_t rslt = 0;
//
//    /* First clear the zero position */
//    rslt = user_i2c_write(AS5048B_PHYSICAL_ADDR, REG_ZERO_POS_HIGH, reg_data, 1);
//
//    /* Update registers to get current angle */
//    AS5048B_UpdateRegisters(sensor);
//
//    /* Set the zero position to current angle */
//    reg_data[0] = sensor->registers.angle_high;
//    reg_data[1] = sensor->registers.angle_low;
//    rslt = user_i2c_write(AS5048B_PHYSICAL_ADDR, REG_ZERO_POS_HIGH, reg_data, 2);
//
//    return rslt;
//}
//
///**
// * Gets the current angle in degrees (0-360)
// *
// * @param sensor Pointer to sensor structure
// * @return Angle in degrees as float
// */
//float AS5048B_GetAngleDegrees(AS5048B_Sensor *sensor)
//{
//    /* Update registers to get current angle */
//    AS5048B_UpdateRegisters(sensor);
//
//    /* Calculate angle in degrees */
//    uint16_t raw_angle = (sensor->registers.angle_high << 6) | sensor->registers.angle_low;
//    float angle_degrees = (float)raw_angle * 360.0f / 16384.0f; /* 14-bit resolution (2^14 = 16384) */
//
//    return angle_degrees;
//}
//
///**
// * Gets the current angle in radians (0-2π)
// *
// * @param sensor Pointer to sensor structure
// * @return Angle in radians as float
// */
//float AS5048B_GetAngleRadians(AS5048B_Sensor *sensor)
//{
//    float angle_degrees = AS5048B_GetAngleDegrees(sensor);
//    float angle_radians = angle_degrees * 3.14159265359f / 180.0f;
//
//    return angle_radians;
//}
//
///**
// * Gets the current magnitude (signal strength)
// *
// * @param sensor Pointer to sensor structure
// * @return Magnitude value (0-4095)
// */
//uint16_t AS5048B_GetMagnitude(AS5048B_Sensor *sensor)
//{
//    /* Update registers to get current magnitude */
//    AS5048B_UpdateRegisters(sensor);
//
//    /* Calculate magnitude */
//    uint16_t magnitude = (sensor->registers.magnitude_high << 6) | sensor->registers.magnitude_low;
//
//    return magnitude;
//}
//
///**
// * Checks diagnostic information
// *
// * @param sensor Pointer to sensor structure
// * @return 0 if no errors, error code otherwise
// */
//uint8_t AS5048B_CheckDiagnostics(AS5048B_Sensor *sensor)
//{
//    /* Update registers to get current diagnostics */
//    AS5048B_UpdateRegisters(sensor);
//
//    return sensor->registers.diagnostics;
//}
//
