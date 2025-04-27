/**
 * AS5048B Magnetic Position Sensor Driver
 *
 * Description:
 * The host MCU (master) initiates all data transfers.
 * The 7-bit slave device address depends on the state of
 * OTP I²C register 21 (0x15) bits 0-4 plus 2 I²C address
 * selection pins (3 and 4).
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"  /* STM32F4 HAL library for I2C communication */
#include "main.h"       /* For Chip Select pin and port definitions */
#include "stdint.h"

/*********************
 * External variables
 *********************/
extern I2C_HandleTypeDef hi2c1;

/* I2C Device Addresses */
#define AS5048B_I2C_ADDRESS0 0x40 // Default I2C address of the device

/* Register Addresses */
#define REG_PROG_CTRL            0x03    // Programming Control register
#define REG_I2C_ADDR             0x15    // I2C slave address register
#define REG_ZERO_POS_HIGH        0x16    // Zero Position High byte register
#define REG_ZERO_POS_LOW         0x17    // Zero Position Low byte register
#define REG_AGC                  0xFA    // Automatic Gain Control register
#define REG_DIAG                 0xFB    // Diagnostics register
#define REG_MAGNITUDE_HIGH       0xFC    // Magnitude High byte register
#define REG_MAGNITUDE_LOW        0xFD    // Magnitude Low byte register
#define REG_ANGLE_HIGH           0xFE    // Angle High byte register
#define REG_ANGLE_LOW            0xFF    // Angle Low byte register

/**
 * AS5048B Register Structure
 * Bit fields organized according to the datasheet
 */
typedef struct {
    /* Readout Registers */
    uint8_t angle_low : 6;               // Lower 6 bits of angle value
    uint8_t angle_high;                  // Upper 8 bits of angle value
    uint8_t magnitude_low : 6;           // Lower 6 bits of magnitude value
    uint8_t magnitude_high;              // Upper 8 bits of magnitude value
    uint8_t diagnostics : 4;             // Diagnostic flags
    uint8_t automatic_gain_control;      // AGC value

    /* Programmable Customer Settings */
    uint8_t zero_position_low : 6;       // Lower 6 bits of zero position
    uint8_t zero_position_high;          // Upper 8 bits of zero position
    uint8_t i2c_slave_address : 5;       // 5-bit I2C slave address

    /* OTP Control */
    uint8_t programming_control : 7;     // Programming control bits

} AS5048B_Registers;

/**
 * Main sensor structure holding all sensor data
 */
typedef struct {
    AS5048B_Registers registers;
} AS5048B_Sensor;

/* Function Prototypes */
/**
 * Sets the mechanical zero position of the sensor
 *
 * @param sensor Pointer to sensor structure
 * @return Status code (0 for success)
 */
int AS5048B_SetZeroPosition(AS5048B_Sensor *sensor);

/**
 * Updates all register values from the sensor
 *
 * @param sensor Pointer to sensor structure
 * @return Status code (0 for success)
 */
int AS5048B_UpdateRegisters(AS5048B_Sensor *sensor);

/**
 * Gets the current angle in degrees (0-360)
 *
 * @param sensor Pointer to sensor structure
 * @return Angle in degrees as float
 */
float AS5048B_GetAngleDegrees(AS5048B_Sensor *sensor);

/**
 * Gets the current angle in radians (0-2π)
 *
 * @param sensor Pointer to sensor structure
 * @return Angle in radians as float
 */
float AS5048B_GetAngleRadians(AS5048B_Sensor *sensor);

/**
 * Gets the current magnitude (signal strength)
 *
 * @param sensor Pointer to sensor structure
 * @return Magnitude value (0-4095)
 */
uint16_t AS5048B_GetMagnitude(AS5048B_Sensor *sensor);

/**
 * Checks diagnostic information
 *
 * @param sensor Pointer to sensor structure
 * @return 0 if no errors, error code otherwise
 */
uint8_t AS5048B_CheckDiagnostics(AS5048B_Sensor *sensor);
