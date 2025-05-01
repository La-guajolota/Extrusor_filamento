/*
 * AS5048B Magnetic Position Sensor Driver
 *
 * Description:
 *   The host MCU (master) initiates all data transfers.
 *   The 7-bit slave device address depends on the state of
 *   OTP I²C register 21 (0x15) bits 0-4 plus 2 I²C address
 *   selection pins (3 and 4).
 */

#ifndef AS5048B_DRIVER_H
#define AS5048B_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"       /* STM32F4 HAL library */
#include "main.h"            /* Board-specific definitions */
#include <stdint.h>

/* I2C Device Limits --------------------------------------------------------*/
#define AS5048B_MAX_DEVICES     2
#define MAX_I2C_ADDR           127
#define AS5048B_DEFAULT_ADDR   0x40U  /* Default 7-bit I2C address */

/* AS5048B Register Addresses ----------------------------------------------*/
enum {
    REG_PROG_CTRL        = 0x03,
    REG_I2C_ADDR         = 0x15,
    REG_ZERO_POS_HIGH    = 0x16,
    REG_ZERO_POS_LOW     = 0x17,
    REG_AGC              = 0xFA,
    REG_DIAG             = 0xFB,
    REG_MAGNITUDE_HIGH   = 0xFC,
    REG_MAGNITUDE_LOW    = 0xFD,
    REG_ANGLE_HIGH       = 0xFE,
    REG_ANGLE_LOW        = 0xFF
};

/* AS5048B Register Map -----------------------------------------------------*/
typedef struct {
    /* Measurement Output */
    uint8_t angle_low       : 6;  /* 6 LSBs of angle */
    uint8_t                  : 2;  /* padding */
    uint8_t angle_high;            /* 8 MSBs of angle */

    uint8_t magnitude_low   : 6;  /* 6 LSBs of magnitude */
    uint8_t                  : 2;  /* padding */
    uint8_t magnitude_high;        /* 8 MSBs of magnitude */

    uint8_t diagnostics     : 4;  /* diagnostic flags */
    uint8_t automatic_gain_control;/* AGC value */

    /* Customer Settings */
    uint8_t zero_pos_low    : 6;  /* 6 LSBs of zero position */
    uint8_t                  : 2;  /* padding */
    uint8_t zero_pos_high;         /* 8 MSBs of zero position */
    uint8_t i2c_slave_addr  : 5;  /* programmable I2C address */
    uint8_t                  : 3;  /* padding */

    /* OTP Programming */
    uint8_t prog_ctrl       : 7;  /* programming control bits */
    uint8_t                  : 1;  /* padding */
} AS5048B_Registers;

/* Sensor Descriptor --------------------------------------------------------*/
typedef struct {
    AS5048B_Registers registers;   /**< Register cache */
    uint8_t dev_id;                /**< 7-bit I2C address */
} AS5048B_Sensor;

/* Driver Control Structure ------------------------------------------------*/
typedef struct {
    I2C_HandleTypeDef *hi2c;                   /**< I2C handle */
    AS5048B_Sensor     devices[AS5048B_MAX_DEVICES]; /**< Sensor array */
    uint8_t            device_count;           /**< Found devices */
} AS5048B_Driver_t;

/* Public API ----------------------------------------------------------------*/

/**
 * @brief Initialize AS5048B driver
 * @param driver   Pointer to driver control struct
 * @param hi2c     I2C handle
 * @return HAL status
 */
HAL_StatusTypeDef AS5048B_Init(AS5048B_Driver_t *driver,
                               I2C_HandleTypeDef *hi2c);

/**
 * @brief Add sensor to driver
 * @param driver      Pointer to driver struct
 * @param num_encoder Index in driver->devices[]
 * @param dev_id      7-bit I2C address
 * @return HAL status
 */
HAL_StatusTypeDef AS5048B_AddDevice(AS5048B_Driver_t *driver,
                                    uint8_t num_encoder,
                                    uint8_t dev_id);

/**
 * @brief Scan I2C bus and populate driver->devices[].dev_id
 * @param driver   Pointer to driver struct
 */
void find_dev_id_address(AS5048B_Driver_t *driver);

/**
 * @brief Set mechanical zero position for given encoder
 * @param driver       Pointer to driver struct
 * @param num_encoder  Index in driver->devices[]
 * @return 0 on success
 */
int AS5048B_SetZeroPosition(AS5048B_Driver_t *driver,
                            uint8_t num_encoder);

/**
 * @brief Update register cache for given encoder
 * @param driver       Pointer to driver struct
 * @param num_encoder  Index in driver->devices[]
 * @return 0 on success
 */
int AS5048B_UpdateRegisters(AS5048B_Driver_t *driver,
                            uint8_t num_encoder);

/**
 * @brief Get angle in degrees for given encoder
 * @param driver       Pointer to driver struct
 * @param num_encoder  Index in driver->devices[]
 * @return Angle [0..360)
 */
float AS5048B_GetAngleDegrees(AS5048B_Driver_t *driver,
                              uint8_t num_encoder);

/**
 * @brief Get angle in radians for given encoder
 * @param driver       Pointer to driver struct
 * @param num_encoder  Index in driver->devices[]
 * @return Angle [0..2*PI)
 */
float AS5048B_GetAngleRadians(AS5048B_Driver_t *driver,
                              uint8_t num_encoder);

/**
 * @brief Get signal magnitude for given encoder
 * @param driver       Pointer to driver struct
 * @param num_encoder  Index in driver->devices[]
 * @return Magnitude [0..4095]
 */
uint16_t AS5048B_GetMagnitude(AS5048B_Driver_t *driver,
                              uint8_t num_encoder);

/**
 * @brief Check diagnostic flags for given encoder
 * @param driver       Pointer to driver struct
 * @param num_encoder  Index in driver->devices[]
 * @return 0 if OK, error code otherwise
 */
uint8_t AS5048B_CheckDiagnostics(AS5048B_Driver_t *driver,
                                  uint8_t num_encoder);

#endif /* AS5048B_DRIVER_H */
