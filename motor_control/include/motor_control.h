#ifndef CONTROL_H
#define CONTROL_H

// STD Libraries
#include <iostream>
#include <cstdio>
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

// Acceleration
#define ACCELERATION_STEP 5.0f  // mm/s per cycle

// Implement PID for tracking correction needed
// Needs to: 
// 1. Compute Error
// 2. Compute PID Terms
// 3. Compute Final Correction
// 4. Save Previous Error for Next Iteration
// Step 5: Clamp to scale useable by motor
float compute_correction();

// Set Motor Speed
// MOTOR_ID --> motor to change
// speed --> speed factor (-1 to 1)
void set_motor_speed(uint8_t MOTOR_ID, float speed);

// Update Velocity
void update_velocity();

// Calculates Updated Motor Speed
float speed_pid();

// Used by main process to track progress on a motor action
void motor_action_tracking(bool &motor_correction);
#endif