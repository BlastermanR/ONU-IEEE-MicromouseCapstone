#ifndef CONTROL_H
#define CONTROL_H

// STD Libraries
#include <iostream>
#include <cmath>
#include <algorithm>

// Pico Libraries
#include <pico/stdlib.h>
#include <hardware/pwm.h>

// Custom Libraries
#include <platform_config.h>
#include <shared_data.h>
#include <maze.h>

// Define weights/metrics
#define GYRO_CORRECTION_WEIGHT 0.05
#define SENSOR_CORRECTION_WEIGHT 0.2
#define CORRECTION_DEADBAND_MM 3
#define CRITICAL_DISTANCE_MM 5
#define ROTATION_OFFSET 0.05
// Find Motor Correction
float compute_correction();

// Set Motor Speed
// MOTOR_ID --> motor to change
// speed --> speed factor (-1 to 1)
void set_motor_speed(uint8_t MOTOR_ID, float speed);

#endif