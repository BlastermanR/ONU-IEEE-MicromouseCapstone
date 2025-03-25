#ifndef INTURRUPTS_H
#define INTURRUPTS_H

#include <pico/stdlib.h>
#include <platform_config.h>
#include <pico/time.h>
#include <hardware/clocks.h>
#include <hardware/pwm.h>
#include <hardware/irq.h>
#include <hardware/timer.h>

// Global Declarations
#define GYRO_TIMER_MS 10
#define CORRECTION_TIMER_MS 25

// Flags
extern volatile bool ir_sensor_ready_flag_1;
extern volatile bool ir_sensor_ready_flag_2;
extern volatile bool ir_sensor_ready_flag_3;
extern volatile bool gyro_sensor_ready_flag;
extern volatile bool sw1_on_flag;
extern volatile bool sw2_on_flag;
extern volatile bool sw3_on_flag;
extern volatile bool motor_correct_flag;

// Timers
extern struct repeating_timer correction_timer;
extern struct repeating_timer gyro_timer;

// Encoder PWM data
extern volatile uint64_t rotation_count[NUM_PWM_PINS]; // Store total # of rotations
extern volatile uint32_t pwm_period[NUM_PWM_PINS]; // Store period for each pin
extern volatile uint32_t last_wrap_time[NUM_PWM_PINS]; // Store last wrap time for each PWM pin
extern volatile uint pwm_slice_num[NUM_PWM_PINS]; // Store pwm slice number

// Interrupt Handlers
void ir_sensor_irq_handler(uint gpio, uint32_t events);

// Gyro Sensor
bool gyro_sensor_irq_handler(struct repeating_timer *t);

// Switches
void switch_irq_handler(uint gpio, uint32_t events);

// Setup handler to process interrupts
void pwm_irq_handler();

// Setups PWM capture on pins
void pwm_capture_setup();

// Setup all Inturrupts
void setup_interrupts();

// Motor Functions
// Handler for calculating correction
bool correction_irq_handler(struct repeating_timer *t);

#endif 