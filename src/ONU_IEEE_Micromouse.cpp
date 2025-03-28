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

    // Status Vars
    int status = 0;
    uint64_t count = 0;

    while(!status || control_process_exit_signal.read()) 
    {
        // Interrupt Flag Checks
        if(ir_sensor_ready_flag_1)
        {
            uint16_t result;
            status |= read_VL53L4CD(VL53L4CD_1, result);
            left_IR_sensor.write(result);
            ir_sensor_ready_flag_1 = false;
        }

        if(ir_sensor_ready_flag_2)
        {
            uint16_t result;
            status |= read_VL53L4CD(VL53L4CD_2, result);
            left_IR_sensor.write(result);
            ir_sensor_ready_flag_2 = false;
        }

        if(ir_sensor_ready_flag_3)
        {
            uint16_t result;
            status |= read_VL53L4CD(VL53L4CD_3, result);
            left_IR_sensor.write(result);
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
        set_correction_timer(calculate_corrections.read());
        int64_t left_encoder_count, right_encoder_count;
        update_encoder_count(left_encoder_count, right_encoder_count);
        motor_action_tracking(motor_correct_flag, left_encoder_count, right_encoder_count);

        // Update encoder data in shared memory
        left_encoder_rotation_demand.write(left_encoder_count);
        right_encoder_rotation_demand.write(right_encoder_count);

        count++;
    }

    // Ensure motors are stopped
    set_motor_speed(MOTOR_LEFT, 0);
    set_motor_speed(MOTOR_RIGHT, 0);

    printf("Main process exited with code: %d on iteration: %d\n", status, count);
    return status;
}
