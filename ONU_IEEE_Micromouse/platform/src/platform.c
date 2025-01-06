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


#include "platform.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

uint8_t VL53L4CD_RdDWord(Dev_t dev, uint16_t registerAddr, uint32_t *value)
{
    uint8_t status = 0;
    uint8_t buffer[4]; // Buffer to store the 4 bytes read from the sensor
    uint8_t regAddr[2] = {
        (registerAddr >> 8) & 0xFF, // High byte of register address
        registerAddr & 0xFF         // Low byte of register address
    };

    // Write the register address to the sensor
    int result = i2c_write_blocking(I2C_PORT, dev, regAddr, 2, true); // Use repeated start
    if (result < 0) {
        status = 1; // Indicate a write error
        return status;
    }

    // Read 4 bytes from the sensor
    result = i2c_read_blocking(I2C_PORT, dev, buffer, 4, false); // No repeated start
    if (result < 0) {
        status = 2; // Indicate a read error
        return status;
    }

    // Combine the 4 bytes into a 32-bit word
    *value = ((uint32_t)buffer[0] << 24) |
             ((uint32_t)buffer[1] << 16) |
             ((uint32_t)buffer[2] << 8) |
             (uint32_t)buffer[3];

    return status; // 0 indicates success
}

uint8_t VL53L4CD_RdWord(Dev_t dev, uint16_t RegisterAdress, uint16_t *value)
{
    uint8_t status = 0;
    uint8_t buffer[2]; // Buffer to store the 2 bytes read from the sensor
    uint8_t regAddr[2] = {
        (RegisterAdress >> 8) & 0xFF, // High byte of register address
        RegisterAdress & 0xFF         // Low byte of register address
    };

    // Write the register address to the sensor
    int result = i2c_write_blocking(I2C_PORT, dev, regAddr, 2, true); // Use repeated start
    if (result < 0) {
        status = 1; // Indicate a write error
        return status;
    }

    // Read 2 bytes from the sensor
    result = i2c_read_blocking(I2C_PORT, dev, buffer, 2, false); // No repeated start
    if (result < 0) {
        status = 2; // Indicate a read error
        return status;
    }

    // Combine the 2 bytes into a 16-bit word
    *value = ((uint16_t)buffer[0] << 8) | (uint16_t)buffer[1];

    return status; // 0 indicates success
}

uint8_t VL53L4CD_RdByte(Dev_t dev, uint16_t RegisterAdress, uint8_t *value)
{
    uint8_t status = 0;
    uint8_t regAddr[2] = {
        (RegisterAdress >> 8) & 0xFF, // High byte of register address
        RegisterAdress & 0xFF         // Low byte of register address
    };

    // Write the register address to the sensor
    int result = i2c_write_blocking(I2C_PORT, dev, regAddr, 2, true); // Use repeated start
    if (result < 0) {
        status = 1; // Indicate a write error
        return status;
    }

    // Read 1 byte from the sensor
    result = i2c_read_blocking(I2C_PORT, dev, value, 1, false); // No repeated start
    if (result < 0) {
        status = 2; // Indicate a read error
        return status;
    }

    return status; // 0 indicates success
}

uint8_t VL53L4CD_WrByte(Dev_t dev, uint16_t RegisterAdress, uint8_t value)
{
    uint8_t status = 0;
    uint8_t buffer[3]; // Buffer to hold the register address and value

    // Prepare the buffer
    buffer[0] = (RegisterAdress >> 8) & 0xFF; // High byte of register address
    buffer[1] = RegisterAdress & 0xFF;        // Low byte of register address
    buffer[2] = value;                        // Data byte to write

    // Write the data to the sensor
    int result = i2c_write_blocking(I2C_PORT, dev, buffer, 3, false); // No repeated start
    if (result < 0) {
        status = 1; // Indicate a write error
        return status;
    }

    return status; // 0 indicates success
}

uint8_t VL53L4CD_WrWord(Dev_t dev, uint16_t RegisterAdress, uint16_t value)
{
    uint8_t status = 0;
    uint8_t buffer[4]; // Buffer to hold the register address and value

    // Prepare the buffer
    buffer[0] = (RegisterAdress >> 8) & 0xFF; // High byte of register address
    buffer[1] = RegisterAdress & 0xFF;        // Low byte of register address
    buffer[2] = (value >> 8) & 0xFF;          // High byte of the value
    buffer[3] = value & 0xFF;                 // Low byte of the value

    // Write the data to the sensor
    int result = i2c_write_blocking(I2C_PORT, dev, buffer, 4, false); // No repeated start
    if (result < 0) {
        status = 1; // Indicate a write error
        return status;
    }

    return status; // 0 indicates success
}

uint8_t VL53L4CD_WrDWord(Dev_t dev, uint16_t RegisterAdress, uint32_t value)
{
    uint8_t status = 0;
    uint8_t buffer[6]; // Buffer to hold the register address and 32-bit value

    // Prepare the buffer
    buffer[0] = (RegisterAdress >> 8) & 0xFF; // High byte of register address
    buffer[1] = RegisterAdress & 0xFF;        // Low byte of register address
    buffer[2] = (value >> 24) & 0xFF;         // Most significant byte of value
    buffer[3] = (value >> 16) & 0xFF;         // Second byte of value
    buffer[4] = (value >> 8) & 0xFF;          // Third byte of value
    buffer[5] = value & 0xFF;                 // Least significant byte of value

    // Write the data to the sensor
    int result = i2c_write_blocking(I2C_PORT, dev, buffer, 6, false); // No repeated start
    if (result < 0) {
        status = 1; // Indicate a write error
        return status;
    }

    return status; // 0 indicates success
}

uint8_t VL53L4CD_WaitMs(Dev_t dev, uint32_t TimeMs)
{
    uint8_t status = 0;

    // Use the sleep_ms function from the Pico SDK
    sleep_ms(TimeMs);

    return status; // 0 indicates success
}
