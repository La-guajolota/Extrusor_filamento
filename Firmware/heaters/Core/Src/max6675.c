/**
 * @file      max6675.c
 * @author    Adrian Silva Palafox
 * @brief     MAX6675 Thermocouple SPI Interface Driver Implementation
 * @version   1.1
 * @date      April 5, 2024
 *
 * @details   Implementation of the MAX6675 thermocouple interface driver
 *            with support for multiple devices using different CS pins.
 */

#include "max6675.h"

/**
 * @brief Initialize the MAX6675 driver
 *
 * @param driver Pointer to driver control structure
 * @param hspi   Pointer to the SPI handle for communication
 * @return HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef MAX6675_Init(MAX6675_Driver_t *driver, SPI_HandleTypeDef *hspi)
{
    /* Validate input parameters */
    if (driver == NULL || hspi == NULL) {
        return HAL_ERROR;
    }

    /* Initialize driver structure */
    driver->hspi = hspi;
    driver->device_count = 0;

    /* Define CS port array */
    GPIO_TypeDef* cs_ports[] = MAX6675_CS_PORTS;
    uint16_t cs_pins[] = MAX6675_CS_PINS;

    /* Copy CS port/pin information to driver structure */
    for (uint8_t i = 0; i < MAX6675_MAX_DEVICES; i++) {
        driver->cs_ports[i] = cs_ports[i];
        driver->cs_pins[i] = cs_pins[i];

        /* Set all CS pins high (inactive) */
        HAL_GPIO_WritePin(driver->cs_ports[i], driver->cs_pins[i], GPIO_PIN_SET);
    }

    return HAL_OK;
}

/**
 * @brief Add a new MAX6675 device to the driver
 *
 * @param driver    Pointer to driver control structure
 * @param device_id Device ID (0-3) corresponding to CS pin index
 * @return HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef MAX6675_AddDevice(MAX6675_Driver_t *driver, uint8_t device_id)
{
    /* Validate input parameters */
    if (driver == NULL || device_id >= MAX6675_MAX_DEVICES) {
        return HAL_ERROR;
    }

    /* Check if we have room for more devices */
    if (driver->device_count >= MAX6675_MAX_DEVICES) {
        return HAL_ERROR;
    }

    /* Initialize device structure */
    driver->devices[device_id].id = device_id;
    driver->devices[device_id].raw_data = 0;
    driver->devices[device_id].temperature = 0.0f;
    driver->devices[device_id].is_connected = 0;

    /* Increment device count */
    driver->device_count++;

    /* Validate device communication by reading temperature */
    return MAX6675_ReadTemperature(driver, device_id);
}

/**
 * @brief Read temperature data from a specific MAX6675 device
 *
 * @param driver    Pointer to driver control structure
 * @param device_id Device ID (0-3) to read from
 * @return HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef MAX6675_ReadTemperature(MAX6675_Driver_t *driver, uint8_t device_id)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint16_t raw_temp = 0;
    uint8_t data[2] = {0};  /* Buffer for raw data from MAX6675 */

    /* Validate input parameters */
    if (driver == NULL || device_id >= MAX6675_MAX_DEVICES) {
        return HAL_ERROR;
    }

    /* Begin SPI communication sequence */
    HAL_GPIO_WritePin(
        driver->cs_ports[device_id],
        driver->cs_pins[device_id],
        GPIO_PIN_RESET
    ); /* Assert CS (active low) */

    /* Read (16 bits) from the MAX6675 */
    status = HAL_SPI_Receive(driver->hspi, data, 1, 50);

    /* End SPI communication sequence */
    HAL_GPIO_WritePin(
        driver->cs_ports[device_id],
        driver->cs_pins[device_id],
        GPIO_PIN_SET
    ); /* Deassert CS */

    // NEEDED TO MAKE SURE CLOCK SETS HIGH-IDLE AND SLAVE MISO GOES HI-Z
    for (int i = 0; i < 25; i++) __NOP();

    /* Check if SPI communication was successful */
    if (status != HAL_OK) {
        driver->devices[device_id].is_connected = 0;
        return status;
    }

    /* Combine the two bytes into a 16-bit value */
    driver->devices[device_id].raw_data = (data[1] << 8) | data[0];

    /*
     * Verify device integrity by checking:
     * 1. Thermocouple input bit (should be 0 if connected)
     * 2. Dummy bit (should be 0 for proper operation)
     * 3. Full zeros??? ambient's temperature is present
     */
    if ((((driver->devices[device_id].raw_data & MAX6675_INPUT_BIT) >> 2) ==
        ((driver->devices[device_id].raw_data & MAX6675_DUMMY_BIT) >> 15)) &&
        (driver->devices[device_id].raw_data != 0x0000)) {

        /* Extract temperature data (12-bit value shifted right by 3) */
        raw_temp = (driver->devices[device_id].raw_data & MAX6675_TEMP_BITS) >> 3;

        /* Convert to Celsius (0.25°C per count) */
        driver->devices[device_id].temperature = raw_temp * MAX6675_TEMP_FACTOR;
        driver->devices[device_id].is_connected = 1;
    } else {
        /* No thermocouple detected or communication error */
        driver->devices[device_id].temperature = -404.0;
        driver->devices[device_id].is_connected = 0;
        status = HAL_ERROR;
    }

    return status;
}

/**
 * @brief Get the temperature value from a specific MAX6675 device
 *
 * @param driver      Pointer to driver control structure
 * @param device_id   Device ID (0-3) to get temperature from
 * @param temperature Pointer to store the temperature value
 * @return HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef MAX6675_GetTemperature(MAX6675_Driver_t *driver, uint8_t device_id, float *temperature)
{
    /* Validate input parameters */
    if (driver == NULL || device_id >= MAX6675_MAX_DEVICES || temperature == NULL) {
        return HAL_ERROR;
    }

    /* Check if device is connected */
    if (!driver->devices[device_id].is_connected) {
        return HAL_ERROR;
    }

    /* Get temperature value */
    *temperature = driver->devices[device_id].temperature;

    return HAL_OK;
}

/**
 * @brief Check if a specific MAX6675 device is connected
 *
 * @param driver    Pointer to driver control structure
 * @param device_id Device ID (0-3) to check
 * @return uint8_t  1 if connected, 0 if not connected or invalid device ID
 */
uint8_t MAX6675_IsConnected(MAX6675_Driver_t *driver, uint8_t device_id)
{
    /* Validate input parameters */
    if (driver == NULL || device_id >= MAX6675_MAX_DEVICES) {
        return 0;
    }

    return driver->devices[device_id].is_connected;
}
