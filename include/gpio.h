#ifndef GPIO_H
#define GPIO_H

// STD Libraries
#include <platform_config.h>

// VL53LCD Libary
#include <sensor_api.h>

// Pico SDK Libraries
#include <pico/stdlib.h>
#include <hardware/i2c.h>
#include <hardware/i2c.h>
#include <hardware/pwm.h>
#include <iostream>

// Function initializes GPIO pins
void define_gpio();

// Custom I2C Functions
// Sets up I2C addresses to avoid conflicts
void configure_i2c();

// Set IR Sensor IDs
bool configure_ir_address();

// Scans for valid devices
void i2c_scan();

// Tests if device is found at provided addres
bool is_i2c_valid(uint8_t address);

// VL53L4CD Setup
bool VL53L4CD_setup();

#endif
