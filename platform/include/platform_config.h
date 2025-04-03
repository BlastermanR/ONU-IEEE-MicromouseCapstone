/**
 * @file platform_config.h
 * @author Ryan Massie
 * @brief 
 * @version 0.1
 * @date 2024-2-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

#include <stdint.h>

// Clock
#define CLOCK_SPEED 125000000
#define PWM_CLOCK_DIVIDER 4.0
#define PWM_CLOCK_TOP 25000

// Timer Deffs
#define GYRO_TIMER_MS 10
#define ENCODER_TIMER_MS 5
#define CORRECTION_TIMER_MS 15
#define VELOCITY_TIMER_MS 5

// I2C Definitions
#define I2C_PORT i2c0
#define I2C_CLOCK 400000
// I2C pins
#define I2C_SDA 4
#define I2C_SCL 5
// IR Sensor Addresses
#define VL53L4CD_SENSOR_COUNT 3
#define VL53L4CD_ID 0x29
#define VL53L4CD_1 0x30
#define VL53L4CD_2 0x32
#define VL53L4CD_3 0x34
extern const uint8_t VL53L4CD_ADDRESSES[VL53L4CD_SENSOR_COUNT];

// GPIO Port Definitions
// Sensor pins
#define S1_XSHUT 0
#define S2_XSHUT 1
#define S3_XSHUT 2
extern const uint8_t IR_XSHUT_PINS[VL53L4CD_SENSOR_COUNT];
#define S1_GPIO 3
#define S2_GPIO 6
#define S3_GPIO 7
extern const uint8_t IR_GPIO_PINS[VL53L4CD_SENSOR_COUNT];
// LEDs
#define LED1 10
#define LED2 11
#define LED3 12
// Switches
#define SW1 13
#define SW2 14
#define SW3 15
// Motor
#define AIN1 26
#define AIN2 22
#define BIN1 28
#define BIN2 27
#define MOTOR_LEFT 0
#define MOTOR_RIGHT 1
// Encoder
#define CAL_LOGIC 19
#define CBL_LOGIC 18
#define CAR_LOGIC 17
#define CBR_LOGIC 16
#define NUM_PWM_PINS 4
extern const int PWM_PINS[NUM_PWM_PINS];

// Gyro Address
#define MPU6050 0x68

// Define Measurements
#define ROBOT_WIDTH_MM 10000
#define ROBOT_LENGTH_MM 10000
#define IR_PLACEMENT_WIDTH_MM 55.0
#define MAX_IR_RANGE_MM 1200.0

#define ENCODER_NUM_LINES_ROTATION 16384
#define EXTERNAL_GEAR_RATIO  4 // 4 motor turns to 1 wheel turn
#define WHEEL_DIAMETER_MM 20
#define WHEEL_CIRCUMFERENCE_MM 75.4
#define WHEEL_BASE_MM 50 // FIX FOR RIGHT VALUE


#endif