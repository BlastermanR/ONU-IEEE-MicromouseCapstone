/**
 * @file ONU_IEEE_Micromouse.cpp
 * @author Ryan Massie
 * @brief 
 * @version 0.1
 * @date 2024-2-28
 * 
 * @copyright Copyright (c) 2024
 *
 */

// STD Libraries
#include <iostream>
#include <algorithm>

// VL53L4CD Drivers
#include <sensor_api.h>

// Pico SDK Libraries
#include <pico/stdlib.h>
#include <hardware/i2c.h>
#include <pico/multicore.h>

// Custom Code
#include <platform_config.h>
#include <interrupts.h>
#include <gpio.h>
#include <shared_data.h>
#include <motor_control.h>
#include <control_process.h>

void calculate_motor_speeds()
{
    // Calculate Speed
    // V = f(Hz) * 60 / N = 60 / period(s) * N
    float speed_left_mms = 1000.0 / pwm_period[3] * (MM_TRAVELED_PER_ROTATION / ENCODER_NUM_LINES_ROTATION);
    float speed_right_mms = 1000.0 / pwm_period[1] * (MM_TRAVELED_PER_ROTATION / ENCODER_NUM_LINES_ROTATION);

    // Convert to milimeters per second and write to shared memory
    left_motor_speed_mms.write(speed_left_mms);
    right_motor_speed_mms.write(speed_right_mms);
}

// Process handles fast, time sensative operations such as interrupts
int main()
{
    // Configure gpio pins
    define_gpio();

    // Set Switches
    sw1.write(gpio_get(SW1));
    sw2.write(gpio_get(SW2));
    sw3.write(gpio_get(SW3));

    // Define Interrupts
    configure_i2c();

    // Set VL53L4CD IR Sensor Addresses, Inititate Sensors
    if (!configure_ir_address() || !VL53L4CD_setup()) 
    {
        printf("CRITICAL FAILURE: Exiting Program\n");
        return -1;
    }

    // Print I2C Devices
    i2c_scan();

    // Launch 2nd Core --> Handles time non critical tasks
    printf("Core 1: Launching assistant process");
    multicore_launch_core1(control_process);
    printf("Core 1: Launched assistant process");

    // Enable Inturrupts
    setup_interrupts();

    // Loop
    printf("Core 1: Entering Time Sensitive Mode, Printing Limited\n");
    // Vars
    int status = 0;
    uint64_t count = 0;
    bool motor_action_in_progress = false;
    float left_motor_action_start_count;
    float right_motor_action_start_count;
    while(!status || control_process_exit_signal.read()) 
    {
        // Interrupt Flag Checks
        if(ir_sensor_ready_flag_1)
        {
            // I2C Read
            VL53L4CD_ResultsData_t* results;
            status = VL53L4CD_GetResult(VL53L4CD_1, results);
            // Write to shared data
            left_IR_sensor.write(results->distance_mm);
            delete(results);
            status = VL53L4CD_ClearInterrupt(VL53L4CD_1);
            ir_sensor_ready_flag_1 = false;
        }

        if(ir_sensor_ready_flag_2)
        {
            // I2C Read
            VL53L4CD_ResultsData_t* results;
            status = VL53L4CD_GetResult(VL53L4CD_1, results);
            // Write to shared data
            middle_IR_sensor.write(results->distance_mm);
            delete(results);
            status = VL53L4CD_ClearInterrupt(VL53L4CD_2);
            ir_sensor_ready_flag_2 = false;
        }

        if(ir_sensor_ready_flag_3)
        {
            // I2C Read
            VL53L4CD_ResultsData_t* results;
            status = VL53L4CD_GetResult(VL53L4CD_1, results);
            // Write to shared data
            right_IR_sensor.write(results->distance_mm);
            delete(results);
            status = VL53L4CD_ClearInterrupt(VL53L4CD_3);
            ir_sensor_ready_flag_3 = false;
        }

        if(gyro_sensor_ready_flag) {
            // I2C Read
            uint8_t buffer[14];  // All accel, temp, gyro
            uint8_t reg = 0x3B;  // Start at accel X high byte
            i2c_read_blocking(i2c0, MPU6050, buffer, 14, false);
            // Write to shared data
            gyro_data.write_data(buffer);
            gyro_sensor_ready_flag = false;
        }

        // Write Switch Values
        if (sw1_on_flag) {sw1.write(true);}
        else {sw1.write(false);}

        if (sw2_on_flag) {sw2.write(true);}
        else {sw2.write(false);}

        if (sw3_on_flag) {sw3.write(true);}
        else {sw3.write(false);}

        // Update motor speed/rotations/corrections
        if (motor_action_in_progress)
        {
            // Calculate correction if needed
            if (motor_correct_flag)
            {
                float correction = compute_correction();
                // Set motor
                set_motor_speed(MOTOR_LEFT, left_motor_set_speed.read() - correction);
                set_motor_speed(MOTOR_RIGHT, right_motor_set_speed.read() + correction);

                // Reset flag
                motor_correct_flag = false;
            }        
            
            // Check progress
            // Left motor
            float left_count = rotation_count[3] - left_motor_action_start_count + ROTATION_OFFSET;
            float right_count = rotation_count[1] - right_motor_action_start_count + ROTATION_OFFSET;    
            if (left_count > left_motor_rotations.read() || right_count > right_motor_rotations.read()) // Done
            {
                // Disable motors
                set_motor_speed(MOTOR_LEFT, 0);
                set_motor_speed(MOTOR_RIGHT, 0);
                
                // Set flags
                motor_action_in_progress = false;
                motor_action.write(false);
            }
        }   
        // Start Motor
        else if (motor_action.read())
        {
            // Log starting position
            left_motor_action_start_count = rotation_count[3];
            right_motor_action_start_count = rotation_count[1];

            // Set motors
            set_motor_speed(MOTOR_LEFT, left_motor_set_speed.read());
            set_motor_speed(MOTOR_RIGHT, right_motor_set_speed.read());
            
            // Set flags
            motor_action_in_progress = true;
        }

        // Update encoder data in shared memory
        left_motor_rotation_count.write(rotation_count[3]);
        right_motor_rotation_count.write(rotation_count[1]);
        calculate_motor_speeds();

        count++;
    }

    // Ensure motors are stopped
    set_motor_speed(MOTOR_LEFT, 0);
    set_motor_speed(MOTOR_RIGHT, 0);

    printf("Main process exited with code: %d on iteration: %d\n", status, count);
    return status;
}
