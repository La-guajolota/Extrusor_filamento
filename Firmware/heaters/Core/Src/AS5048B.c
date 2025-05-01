/**
 * AS5048B Magnetic Position Sensor Driver Implementation
 *
 * This file contains the implementation of the driver functions
 * for the AS5048B magnetic position sensor using I2C communication.
 */

#include "AS5048B.h"

/* Private I2C helpers ------------------------------------------------------*/
static HAL_StatusTypeDef user_i2c_read(AS5048B_Driver_t *drv,
                                      uint8_t dev_id,
                                      uint8_t reg_addr,
                                      uint8_t *reg_data,
                                      uint16_t len)
{
    uint16_t addr = dev_id << 1;
    HAL_StatusTypeDef rslt;

    rslt = HAL_I2C_Master_Transmit(drv->hi2c, addr, &reg_addr, 1, HAL_MAX_DELAY);
    if (rslt != HAL_OK) return rslt;
    return HAL_I2C_Master_Receive(drv->hi2c, addr, reg_data, len, HAL_MAX_DELAY);
}

static HAL_StatusTypeDef user_i2c_write(AS5048B_Driver_t *drv,
                                       uint8_t dev_id,
                                       uint8_t reg_addr,
                                       uint8_t *reg_data,
                                       uint16_t len)
{
    uint16_t addr = dev_id << 1;
    HAL_StatusTypeDef rslt;

    rslt = HAL_I2C_Master_Transmit(drv->hi2c, addr, &reg_addr, 1, HAL_MAX_DELAY);
    if (rslt != HAL_OK) return rslt;
    return HAL_I2C_Master_Transmit(drv->hi2c, addr, reg_data, len, HAL_MAX_DELAY);
}

/* Public API ---------------------------------------------------------------*/
HAL_StatusTypeDef AS5048B_Init(AS5048B_Driver_t *driver,
                               I2C_HandleTypeDef *hi2c)
{
    if (!driver || !hi2c) return HAL_ERROR;
    driver->hi2c = hi2c;
    driver->device_count = 0;
    return HAL_OK;
}

HAL_StatusTypeDef AS5048B_AddDevice(AS5048B_Driver_t *driver,
                                    uint8_t num_encoder,
                                    uint8_t dev_id)
{
    if (!driver || num_encoder >= AS5048B_MAX_DEVICES) return HAL_ERROR;
    if (driver->device_count >= AS5048B_MAX_DEVICES) return HAL_ERROR;

    driver->devices[num_encoder].dev_id = dev_id;
    driver->device_count++;
    return HAL_I2C_IsDeviceReady(driver->hi2c, dev_id << 1, 1, HAL_MAX_DELAY);
}

void find_dev_id_address(AS5048B_Driver_t *driver)
{
    uint8_t found = 0;
    for (uint8_t addr = 0; addr <= MAX_I2C_ADDR && found < driver->device_count; addr++) {
        if (HAL_I2C_IsDeviceReady(driver->hi2c, addr << 1, 1, HAL_MAX_DELAY) == HAL_OK) {
            driver->devices[found++].dev_id = addr;
        }
    }
}

int AS5048B_UpdateRegisters(AS5048B_Driver_t *driver,
                            uint8_t num_encoder)
{
    AS5048B_Sensor *sens = &driver->devices[num_encoder];
    uint8_t buf[10];
    HAL_StatusTypeDef st = HAL_OK;

    st |= user_i2c_read(driver, sens->dev_id, REG_PROG_CTRL, buf, 1);
    st |= user_i2c_read(driver, sens->dev_id, REG_I2C_ADDR, buf + 1, 2);
    st |= user_i2c_read(driver, sens->dev_id, REG_AGC, buf + 4, 5);

    sens->registers.prog_ctrl            = buf[0];
    sens->registers.i2c_slave_addr       = buf[1];
    sens->registers.zero_pos_high        = buf[2];
    sens->registers.zero_pos_low         = buf[3];
    sens->registers.automatic_gain_control = buf[4];
    sens->registers.diagnostics          = buf[5];
    sens->registers.magnitude_high       = buf[6];
    sens->registers.magnitude_low        = buf[7];
    sens->registers.angle_high           = buf[8];
    sens->registers.angle_low            = buf[9] >> 2;

    return (int)st;
}

int AS5048B_SetZeroPosition(AS5048B_Driver_t *driver,
                            uint8_t num_encoder)
{
    AS5048B_Sensor *sens = &driver->devices[num_encoder];
    uint8_t data[2] = {0,0};
    HAL_StatusTypeDef st;

    st = user_i2c_write(driver, sens->dev_id, REG_ZERO_POS_HIGH, data, 1);
    AS5048B_UpdateRegisters(driver, num_encoder);
    data[0] = sens->registers.angle_high;
    data[1] = sens->registers.angle_low;
    st |= user_i2c_write(driver, sens->dev_id, REG_ZERO_POS_HIGH, data, 2);
    return (int)st;
}

float AS5048B_GetAngleDegrees(AS5048B_Driver_t *driver,
                              uint8_t num_encoder)
{
    AS5048B_Sensor *sens = &driver->devices[num_encoder];
    uint8_t data[2];
    if (user_i2c_read(driver, sens->dev_id, REG_ANGLE_HIGH, data, 2) != HAL_OK)
        return -1.0f;

    uint16_t raw = ((uint16_t)data[0] << 6) | (data[1] >> 2);
    return raw * 360.0f / 16384.0f;
}

float AS5048B_GetAngleRadians(AS5048B_Driver_t *driver,
                              uint8_t num_encoder)
{
    return AS5048B_GetAngleDegrees(driver, num_encoder) * 3.14159265359f / 180.0f;
}

uint16_t AS5048B_GetMagnitude(AS5048B_Driver_t *driver,
                              uint8_t num_encoder)
{
    AS5048B_UpdateRegisters(driver, num_encoder);
    AS5048B_Sensor *sens = &driver->devices[num_encoder];
    return (sens->registers.magnitude_high << 6) | sens->registers.magnitude_low;
}

uint8_t AS5048B_CheckDiagnostics(AS5048B_Driver_t *driver,
                                  uint8_t num_encoder)
{
    AS5048B_UpdateRegisters(driver, num_encoder);
    return driver->devices[num_encoder].registers.diagnostics;
}

