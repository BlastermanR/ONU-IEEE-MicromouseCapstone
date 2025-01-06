/**
 * @file PicoDefs.h
 * @author Ryan Massie (ryanbmassie@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef PicoDefs
#define PicoDefs
#pragma once

#include <stdint.h>

// Define I2C bus pins
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5
#define I2C_PORT i2c0

// Define default sensor address
#define VL53L4CD_ID 0x52

// Define Sensor Object
// Define Sensor Object
struct VL53L4CD_Sensor {
    uint16_t address;
    uint8_t XSHUT;
    uint8_t GPIO;
};

// Function to initialize the sensor
void VL53L4CD_Sensor_init(struct VL53L4CD_Sensor *sensor, uint8_t XSHUT, uint8_t GPIO) {
    sensor->XSHUT = XSHUT;
    sensor->GPIO = GPIO;
    sensor->address = VL53L4CD_ID;  // assuming VL53L4CD_ID is defined elsewhere
}

// Define All IR Sensors
//VL53L4CD_Sensor S2L = VL53L4CD_Sensor(/*TODO*/);
//VL53L4CD_Sensor S1L = VL53L4CD_Sensor();
//VL53L4CD_Sensor S0F = VL53L4CD_Sensor();
//VL53L4CD_Sensor S1R = VL53L4CD_Sensor();
//VL53L4CD_Sensor S2R = VL53L4CD_Sensor();
//VL53L4CD_Sensor SensorArray[5] = {S2L, S1L, S0F, S1L, S1R};

#endif