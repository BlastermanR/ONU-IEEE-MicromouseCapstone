#include <shared_data.h>

// Define Globals
// State/Messages
SharedDataMutex<bool> control_process_exit_signal(false); 
// Switches
SharedDataMutex<bool> sw1(false);
SharedDataMutex<bool> sw2(false);
SharedDataMutex<bool> sw3(false);
// Sensor Data
SharedDataMutex<uint16_t> left_IR_sensor(0);
SharedDataMutex<uint16_t> middle_IR_sensor(0);
SharedDataMutex<uint16_t> right_IR_sensor(0);
GyroReadingMutex gyro_data;
// Encoder Data
SharedDataMutex<int64_t> left_encoder_rotation_demand(0);
SharedDataMutex<int64_t> right_encoder_rotation_demand(0);
// Motor Control
SharedDataMutex<bool> motor_action(false);
SharedDataMutex<bool> calculate_corrections(false);
SharedDataMutex<float> left_motor_set_speed(false);
SharedDataMutex<float> right_motor_set_speed(false);
SharedDataMutex<float> left_motor_rotations(0);
SharedDataMutex<float> right_motor_rotations(0);
