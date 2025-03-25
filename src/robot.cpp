#include <robot.h>

Robot::Robot()
{
    // TODO: Setup values, find starting position
    return;
}

void Robot::move_forward(float distance_mm, CorrectionTypes correction, bool block)
{
    uint64_t intiial_rotations_left = left_motor_rotations.read();
    uint64_t initial_rotations_right = right_motor_rotations.read();    

    // TODO: use encoder rotation data + gyro data to judge distance
}

void Robot::turn(TurnTypes direction, bool block)
{
    // TODO Complete using gyro data / encoders
}

void Robot::calibrate_position()
{
    // TODO Use Sensor/Maze data to update estimated X,Y, theta position
}