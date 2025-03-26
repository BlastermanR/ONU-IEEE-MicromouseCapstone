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
#define GYRO_CORRECTION_WEIGHT 0.2
#define SENSOR_CORRECTION_WEIGHT 0.8
#define CORRECTION_DEADBAND 1.5

// Correction PID coefficients
#define Kp 1.0  // Proportional gain
#define Ki 0.1  // Integral gain
#define Kd 0.05 // Derivative gain

// Implement PID for tracking correction needed
// Needs to: 
// 1. Compute Error
// 2. Compute PID Terms
// 3. Compute Final Correction
// 4. Save Previous Error for Next Iteration
float compute_correction();

// Set Motor Speed
// MOTOR_ID --> motor to change
// speed --> speed factor (-1 to 1)
void set_motor_speed(uint8_t MOTOR_ID, float speed);

// Used by main process to track progress on a motor action
void motor_action_tracking(bool &motor_correction, uint64_t total_left_rotation_count, uint64_t total_right_rotation_count);
#endif