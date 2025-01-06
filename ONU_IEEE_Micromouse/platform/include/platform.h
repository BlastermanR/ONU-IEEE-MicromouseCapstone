/**
 * @file platform.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _PLATFORM_H_
#define _PLATFORM_H_
#pragma once

#include <stdint.h>
#include <string.h>
#include <platform_config.h>

/**
* VL53L4CD device instance.
*/

typedef uint16_t Dev_t;

/**
 * @brief Error instance.
 */
typedef uint8_t VL53L4CD_Error;

/**
 * @brief If the macro below is defined, the device will be programmed to run
 * with I2C Fast Mode Plus (up to 1MHz). Otherwise, default max value is 400kHz.
 */

#define VL53L4CD_I2C_FAST_MODE_PLUS


/**
 * @brief Read 32 bits through I2C.
 */

uint8_t VL53L4CD_RdDWord(Dev_t dev, uint16_t registerAddr, uint32_t *value);

/**
 * @brief Read 16 bits through I2C.
 */

uint8_t VL53L4CD_RdWord(Dev_t dev, uint16_t registerAddr, uint16_t *value);

/**
 * @brief Read 8 bits through I2C.
 */

uint8_t VL53L4CD_RdByte(Dev_t dev, uint16_t registerAddr, uint8_t *value);

/**
 * @brief Write 8 bits through I2C.
 */

uint8_t VL53L4CD_WrByte(Dev_t dev, uint16_t registerAddr, uint8_t value);

/**
 * @brief Write 16 bits through I2C.
 */

uint8_t VL53L4CD_WrWord(Dev_t dev, uint16_t RegisterAdress, uint16_t value);

/**
 * @brief Write 32 bits through I2C.
 */

uint8_t VL53L4CD_WrDWord(Dev_t dev, uint16_t RegisterAdress, uint32_t value);

/**
 * @brief Wait during N milliseconds.
 */

uint8_t VL53L4CD_WaitMs(Dev_t dev, uint32_t TimeMs);

#endif	// _PLATFORM_H_

#ifdef __cplusplus
}
#endif