#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <pico/mutex.h>
#include <atomic>

// Class to allow for varried data syncronization between cores
template <typename T>
class SharedDataMutex {
    private:
    typename std::conditional<std::is_same<T, int64_t>::value, std::atomic<uint32_t>, std::atomic<T>>::type lower;
    typename std::conditional<std::is_same<T, int64_t>::value, std::atomic<uint32_t>, std::atomic<T>>::type upper;

    public:
    SharedDataMutex(T init_data) {
        write(init_data);
    }

    T read() const {
        if constexpr (std::is_same<T, int64_t>::value) {
            uint32_t low, high, check;
            do {
                low = lower.load(std::memory_order_acquire);
                high = upper.load(std::memory_order_acquire);
                check = lower.load(std::memory_order_acquire);  // Check for modification
            } while (low != check);  // Retry if lower changed
            return (static_cast<int64_t>(high) << 32) | low;
        } else {
            return lower.load(std::memory_order_acquire);
        }
    }

    void write(T new_data) {
        if constexpr (std::is_same<T, int64_t>::value) {
            uint32_t low = static_cast<uint32_t>(new_data & 0xFFFFFFFF);
            uint32_t high = static_cast<uint32_t>(new_data >> 32);
            lower.store(low, std::memory_order_release);
            upper.store(high, std::memory_order_release);
        } else {
            lower.store(new_data, std::memory_order_release);
        }
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
extern SharedDataMutex<int32_t> left_encoder_rotation_demand; // In encoder Lines (4096 per motor rotation)
extern SharedDataMutex<int32_t> right_encoder_rotation_demand;
// Motor
extern SharedDataMutex<bool> motor_action; // Command to start motor action / signal end
extern SharedDataMutex<bool> calculate_corrections; // Signal for main process to correct motor movement durring linear travel
// Current V
extern SharedDataMutex<float> current_velocity_left_mms_shared;
extern SharedDataMutex<float> current_velocity_right_mms_shared;
// Set V
extern SharedDataMutex<float> target_velocity_left_mms_shared;
extern SharedDataMutex<float> target_velocity_right_mms_shared;
// Desired V
extern SharedDataMutex<float> desired_velocity_left_mms_shared;
extern SharedDataMutex<float> desired_velocity_right_mms_shared;
// Encoder Count
extern SharedDataMutex<int32_t> left_encoder_count_shared;
extern SharedDataMutex<int32_t> right_encoder_count_shared;

#endif