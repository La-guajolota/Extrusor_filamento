/**
 * @file      max6675.h
 * @author    Adrian Silva Palafox
 * @brief     MAX6675 Thermocouple SPI Interface Driver with Multi-Device Support
 * @version   1.1
 * @date      April 5, 2024
 *
 * @details   This driver provides an interface for multiple MAX6675 thermocouple-to-digital
 *            converters using SPI communication protocol. The MAX6675 performs cold-junction
 *            compensation and digitizes the signal from K-type thermocouples.
 *
 * @note      The MAX6675 is a read-only SPI slave device that returns a 12-bit temperature
 *            reading in 1/4 degrees Celsius resolution.
 */

#ifndef INC_MAX6675_H_
#define INC_MAX6675_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"  /* STM32F4 HAL library for SPI communication */
#include "main.h"       /* For Chip Select pin and port definitions */

/* Configuration Constants --------------------------------------------------*/
/**
 * @brief Maximum number of supported MAX6675 devices
 */
#define MAX6675_MAX_DEVICES     4

/* MAX6675 Chip Select Pin Definitions --------------------------------------*/
/**
 * @brief Array of GPIO ports for all CS pins (do not use directly)
 */
#define MAX6675_CS_PORTS \
    { CS_0_GPIO_Port, CS_1_GPIO_Port, CS_2_GPIO_Port, CS_3_GPIO_Port }

/**
 * @brief Array of GPIO pins for all CS pins (do not use directly)
 */
#define MAX6675_CS_PINS \
    { CS_0_Pin, CS_1_Pin, CS_2_Pin, CS_3_Pin }

/* MAX6675 Register Bit Definitions -----------------------------------------*/
/**
 * @brief Dummy bit (D15) - Should always read as 0
 * @note  Can be used to verify proper communication
 */
#define MAX6675_DUMMY_BIT       0x8000

/**
 * @brief Open circuit detection bit (D2)
 * @note  0 = Thermocouple connected, 1 = Thermocouple open circuit
 */
#define MAX6675_INPUT_BIT       0x0004

/**
 * @brief Temperature data bits (D14-D3)
 * @note  12-bit temperature value
 */
#define MAX6675_TEMP_BITS       0x7FF8

/**
 * @brief Device ID bit (D1)
 * @note  Should always read as 0
 */
#define MAX6675_ID_BIT          0x0002

/**
 * @brief Temperature conversion factor (°C per bit)
 */
#define MAX6675_TEMP_FACTOR     0.25f

/* Type Definitions ---------------------------------------------------------*/
/**
 * @brief MAX6675 device structure
 */
typedef struct
{
    uint8_t  id;         /**< Device ID (0-3, used for CS pin selection) */
    uint16_t raw_data;   /**< Raw 16-bit data from the MAX6675 register */
    float    temperature; /**< Processed temperature reading in Celsius */
    uint8_t  is_connected; /**< Connection status (1=connected, 0=disconnected) */
} MAX6675_Device_t;

/**
 * @brief MAX6675 driver control structure
 */
typedef struct {
    MAX6675_Device_t devices[MAX6675_MAX_DEVICES]; /**< Array of MAX6675 device structures */
    uint8_t device_count;                       /**< Number of initialized devices */
    SPI_HandleTypeDef *hspi;                   /**< SPI handle for communication */
    GPIO_TypeDef* cs_ports[MAX6675_MAX_DEVICES]; /**< Array of CS GPIO ports */
    uint16_t cs_pins[MAX6675_MAX_DEVICES];     /**< Array of CS GPIO pins */
} MAX6675_Driver_t;

/* Function Prototypes ------------------------------------------------------*/
/**
 * @brief   Initialize the MAX6675 driver
 * @param   driver      Pointer to driver control structure
 * @param   hspi        Pointer to the SPI handle for communication
 * @return  HAL_StatusTypeDef   HAL status (HAL_OK, HAL_ERROR)
 */
HAL_StatusTypeDef MAX6675_Init(MAX6675_Driver_t *driver, SPI_HandleTypeDef *hspi);

/**
 * @brief   Add a new MAX6675 device to the driver
 * @param   driver      Pointer to driver control structure
 * @param   device_id   Device ID (0-3) corresponding to CS pin index
 * @return  HAL_StatusTypeDef   HAL status (HAL_OK, HAL_ERROR)
 */
HAL_StatusTypeDef MAX6675_AddDevice(MAX6675_Driver_t *driver, uint8_t device_id);

/**
 * @brief   Read temperature data from a specific MAX6675 device
 * @param   driver      Pointer to driver control structure
 * @param   device_id   Device ID (0-3) to read from
 * @return  HAL_StatusTypeDef   HAL status (HAL_OK, HAL_ERROR)
 */
HAL_StatusTypeDef MAX6675_ReadTemperature(MAX6675_Driver_t *driver, uint8_t device_id);

/**
 * @brief   Get the temperature value from a specific MAX6675 device
 * @param   driver      Pointer to driver control structure
 * @param   device_id   Device ID (0-3) to get temperature from
 * @param   temperature Pointer to store the temperature value
 * @return  HAL_StatusTypeDef   HAL status (HAL_OK, HAL_ERROR)
 */
HAL_StatusTypeDef MAX6675_GetTemperature(MAX6675_Driver_t *driver, uint8_t device_id, float *temperature);

/**
 * @brief   Check if a specific MAX6675 device is connected
 * @param   driver      Pointer to driver control structure
 * @param   device_id   Device ID (0-3) to check
 * @return  uint8_t     1 if connected, 0 if not connected or invalid device ID
 */
uint8_t MAX6675_IsConnected(MAX6675_Driver_t *driver, uint8_t device_id);

#endif /* INC_MAX6675_H_ */
