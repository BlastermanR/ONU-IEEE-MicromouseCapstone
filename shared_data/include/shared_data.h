#ifndef SHARED_DATA_H
#define SHARED_DATA_H

//#include <pico/stdlib.h>
#include <pico/mutex.h>

// Class to allow for varried data syncronization between cores
template <typename T>
class SharedDataMutex 
{
private:
    mutex_t data_mutex;
    T data;

public:
    SharedDataMutex(T data) {
        this->data = data;
        mutex_init(&data_mutex);
    }

    // Standard read with mutex protection
    T read() {
        mutex_enter_blocking(&data_mutex);
        T value = data;
        mutex_exit(&data_mutex);
        return value;
    }

    // Standard write with mutex protection
    void write(T new_data) {
        mutex_enter_blocking(&data_mutex);
        data = new_data;
        mutex_exit(&data_mutex);
    }
};

struct GyroReadingMutex
{
    private:
    mutex_t data_mutex;
    uint32_t save;
    public:
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t gx;
    int16_t gy;
    int16_t gz;

    GyroReadingMutex() {mutex_init(&data_mutex);}

    void get_lock() {mutex_enter_blocking(&data_mutex);}

    void release_lock() {mutex_exit(&data_mutex);}

    void write_data(uint8_t (&buffer)[14]) {
        get_lock();
        ax = (buffer[0] << 8) | buffer[1];
        ay = (buffer[2] << 8) | buffer[3];
        az = (buffer[4] << 8) | buffer[5];
        gx = (buffer[8] << 8) | buffer[9];
        gy = (buffer[10] << 8) | buffer[11];
        gz = (buffer[12] << 8) | buffer[13];
        release_lock();
    }
};

// Define Globals
// State/Messages
extern SharedDataMutex<bool> control_process_exit_signal; // Signal to main process to end
// Switches
extern SharedDataMutex<bool> sw1;
extern SharedDataMutex<bool> sw2;
extern SharedDataMutex<bool> sw3;
// Sensor Data
extern SharedDataMutex<uint16_t> left_IR_sensor;
extern SharedDataMutex<uint16_t> middle_IR_sensor;
extern SharedDataMutex<uint16_t> right_IR_sensor;
extern GyroReadingMutex gyro_data;
// Encoder Data
extern SharedDataMutex<int64_t> left_encoder_rotation_demand; // In encoder Lines (4096 per motor rotation)
extern SharedDataMutex<int64_t> right_encoder_rotation_demand;
// Motor
extern SharedDataMutex<bool> motor_action;
extern SharedDataMutex<bool> calculate_corrections; // Signal for main process to correct motor movement durring linear travel
extern SharedDataMutex<float> left_motor_set_speed; // Speed factor (-1 -> 1)
extern SharedDataMutex<float> right_motor_set_speed;
extern SharedDataMutex<float> left_motor_rotations; // total # of encoder lines
extern SharedDataMutex<float> right_motor_rotations;

#endif