#include <robot.h>

Robot::Robot()
{
    // TODO: Setup values, find starting position
    return;
}

// Instructs main process to move forward, by how much and if correcitons/blocking are used
void Robot::move_forward(float distance_mm, bool block, CorrectionTypes correction)
{
    // Calculate encoder lines
    if (motor_action.read()) {return;} // Abort if motor action in progress
    int64_t encoder_lines = (int64_t)round(distance_mm * ENCODER_NUM_LINES_ROTATION * EXTERNAL_GEAR_RATIO / WHEEL_CIRCUMFERENCE_MM);
    left_encoder_rotation_demand.write(encoder_lines);
    right_encoder_rotation_demand.write(encoder_lines);
    
    // Set Correction
    // TODO: CREATE SYSTEM FOR MODE SWITCHING STRIAGHT/DIAGONAL
    if (correction == NONE) 
    {
        calculate_corrections.write(false);
    }
    else
    {
        calculate_corrections.write(true);
    }

    // Start Motor
    motor_action.write(true);

    // Nested to avoid unecessary mutex action
    while(block && motor_action.read()) {sleep_ms(5);}
}

// Instructs main process to turn, by how much and if correcitons/blocking are used
void Robot::turn(int direction_cw, bool block)
{
    // Calculate encoder lines
    if (motor_action.read()) {return;} // Abort if motor action in progress

    // Calculate Encoder Lines
    int64_t encoder_lines = (int64_t)round(direction_cw / 360 * ENCODER_NUM_LINES_ROTATION * WHEEL_BASE_MM / WHEEL_DIAMETER_MM * EXTERNAL_GEAR_RATIO);
    left_encoder_rotation_demand.write(encoder_lines);
    right_encoder_rotation_demand.write(-1 * encoder_lines);

    // Set Correction
    calculate_corrections.write(false);

    // Start Motor
    motor_action.write(true);

    // Nested to avoid unecessary mutex action
    while(block && motor_action.read()) {sleep_ms(5);}
}

void Robot::calibrate_position()
{
    // TODO Use Sensor/Maze data to update estimated X,Y, theta position
}